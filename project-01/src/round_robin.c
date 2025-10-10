/**
 * Author: Deepak Govindarajan
 * CSC 4320 Operating Systems
 * Project 1: Process Scheduling Simulation
 * 9/25/2025

 * Round Robin scheduling algorithm

 * The RR algorithm assigns a fixed time quantum to each process and cycles through them.
 * This is a preemptive algorithm where processes are switched after their time quantum expires.

 * Input: processes.txt containing process information in format: process_id arrival_time burst_time
 * Output: Gantt chart, execution sequence, waiting time, and turnaround time for each process
 */

#include "round_robin.h"

/**
 * Read process data from file
 * Format: PID Arrival_Time Burst_Time Priority
 */
int read_processes_from_file(const char *filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", filename);
        return 0;
    }

    // Skip header line
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);

    int count = 0;
    int priority; // Read but not used in RR

    // Read each process line
    while (fscanf(file, "%d %d %d %d", &processes[count].id, &processes[count].arrival_time, 
                  &processes[count].burst_time, &priority) == 4) {
        
        // Initialize process status
        processes[count].remaining_time = processes[count].burst_time;
        processes[count].is_completed = false;
        
        // Initialize memory fields
        processes[count].memory_size = 50 + (processes[count].id * 20);
        processes[count].allocated_address = -1;
        processes[count].pages_needed = (processes[count].memory_size + PAGE_SIZE - 1) / PAGE_SIZE;
        
        // Clear page table
        for (int j = 0; j < MAX_PROCESS_PAGES; j++) {
            processes[count].page_table[j] = -1;
        }

        count++;
        if (count >= MAX_PROCESSES) {
            printf("Warning: Reached max process limit\n");
            break;
        }
    }

    fclose(file);
    return count;
}

/**
 * Round Robin scheduling algorithm (preemptive)
 * Uses time quantum to cycle through processes
 */
void round_robin_schedule(Process processes[], int n, GanttEntry gantt[], int *gantt_count) {
    int current_time = 0;
    int completed_processes = 0;
    int queue[MAX_PROCESSES];
    int front = 0, rear = 0;
    bool in_queue[MAX_PROCESSES] = {false};
    *gantt_count = 0;

    // Add processes that arrive at time 0
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time == 0) {
            queue[rear++] = i;
            in_queue[i] = true;
        }
    }

    // Main scheduling loop
    while (completed_processes < n) {
        // If queue is empty, advance time to next arrival
        if (front == rear) {
            int next_arrival = -1;
            for (int i = 0; i < n; i++) {
                if (!processes[i].is_completed && processes[i].arrival_time > current_time) {
                    if (next_arrival == -1 || processes[i].arrival_time < processes[next_arrival].arrival_time) {
                        next_arrival = i;
                    }
                }
            }

            if (next_arrival != -1) {
                current_time = processes[next_arrival].arrival_time;
                queue[rear++] = next_arrival;
                in_queue[next_arrival] = true;
            }
            continue;
        }

        // Get next process from queue
        int current_process = queue[front++];
        Process *p = &processes[current_process];

        // Calculate execution time for this quantum
        int execution_time = (p->remaining_time > TIME_QUANTUM) ? TIME_QUANTUM : p->remaining_time;

        // Record gantt entry
        gantt[*gantt_count].process_id = p->id;
        gantt[*gantt_count].start_time = current_time;
        gantt[*gantt_count].end_time = current_time + execution_time;
        (*gantt_count)++;

        // Execute process
        current_time += execution_time;
        p->remaining_time -= execution_time;

        // Check for newly arrived processes during execution
        for (int i = 0; i < n; i++) {
            if (!processes[i].is_completed && !in_queue[i] &&
                processes[i].arrival_time <= current_time && 
                processes[i].arrival_time > (current_time - execution_time)) {
                queue[rear++] = i;
                in_queue[i] = true;
            }
        }

        // Check if process completed
        if (p->remaining_time == 0) {
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            p->is_completed = true;
            in_queue[current_process] = false;
            completed_processes++;
        } else {
            // Add process back to queue if not completed
            queue[rear++] = current_process;
        }
    }
}

/**
 * Round Robin with memory management integration
 */
void round_robin_with_memory(Process processes[], int n, GanttEntry gantt[], int *gantt_count, 
                             MemoryManager *mm, AllocationAlgorithm alloc_alg, PageReplacementAlgorithm page_alg) {
    int current_time = 0;
    int completed_processes = 0;
    int queue[MAX_PROCESSES];
    int front = 0, rear = 0;
    bool in_queue[MAX_PROCESSES] = {false};
    bool memory_allocated[MAX_PROCESSES] = {false};
    *gantt_count = 0;

    // Add processes that arrive at time 0
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival_time == 0) {
            queue[rear++] = i;
            in_queue[i] = true;
        }
    }

    // Main scheduling loop
    while (completed_processes < n) {
        // If queue is empty, advance time to next arrival
        if (front == rear) {
            int next_arrival = -1;
            for (int i = 0; i < n; i++) {
                if (!processes[i].is_completed && processes[i].arrival_time > current_time) {
                    if (next_arrival == -1 || processes[i].arrival_time < processes[next_arrival].arrival_time) {
                        next_arrival = i;
                    }
                }
            }

            if (next_arrival != -1) {
                current_time = processes[next_arrival].arrival_time;
                queue[rear++] = next_arrival;
                in_queue[next_arrival] = true;
            }
            continue;
        }

        // Get next process from queue
        int current_process = queue[front++];
        Process *p = &processes[current_process];

        // Allocate memory when process first runs
        if (!memory_allocated[current_process]) {
            bool contiguous_allocated = allocate_memory_contiguous(mm, p, alloc_alg);
            bool pages_allocated = allocate_pages(mm, p, page_alg);
            
            if (!contiguous_allocated) {
                printf("Warning: Could not allocate contiguous memory for Process P%d\n", p->id);
            }
            if (!pages_allocated) {
                printf("Warning: Could not allocate pages for Process P%d\n", p->id);
            }
            memory_allocated[current_process] = true;
        }

        // Calculate execution time for this quantum
        int execution_time = (p->remaining_time > TIME_QUANTUM) ? TIME_QUANTUM : p->remaining_time;

        // Record gantt entry
        gantt[*gantt_count].process_id = p->id;
        gantt[*gantt_count].start_time = current_time;
        gantt[*gantt_count].end_time = current_time + execution_time;
        (*gantt_count)++;

        // Simulate memory access during execution
        for (int access = 0; access < p->pages_needed && access < execution_time; access++) {
            if (p->page_table[access] != -1) {
                access_page(mm, p->page_table[access]);
            }
        }

        // Execute process
        current_time += execution_time;
        p->remaining_time -= execution_time;

        // Check for newly arrived processes during execution
        for (int i = 0; i < n; i++) {
            if (!processes[i].is_completed && !in_queue[i] &&
                processes[i].arrival_time <= current_time && 
                processes[i].arrival_time > (current_time - execution_time)) {
                queue[rear++] = i;
                in_queue[i] = true;
            }
        }

        // Check if process completed
        if (p->remaining_time == 0) {
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            p->is_completed = true;
            in_queue[current_process] = false;
            completed_processes++;
            
            // Deallocate memory when process completes
            deallocate_memory_contiguous(mm, p);
            deallocate_pages(mm, p);
        } else {
            // Add process back to queue if not completed
            queue[rear++] = current_process;
        }
    }
}

/**
 * Print gantt chart
 */
void print_gantt_chart(GanttEntry gantt[], int gantt_count) {
    printf("\nGantt Chart:\n");
    printf("|");
    for (int i = 0; i < gantt_count; i++) {
        printf(" P%d |", gantt[i].process_id);
    }
    printf("\n");
    printf("%d", gantt[0].start_time);
    for (int i = 0; i < gantt_count; i++) {
        printf("    %d", gantt[i].end_time);
    }
    printf("\n\n");
}

/**
 * Print scheduling results
 */
void print_scheduling_results(Process processes[], int n) {
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    printf("Process Statistics:\n");
    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");
    printf("---\t-------\t-----\t-------\t----------\n");

    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n",
               processes[i].id,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].waiting_time,
               processes[i].turnaround_time);

        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    printf("\nAverage Waiting Time:    %.2f\n", (double)total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", (double)total_turnaround_time / n);
}

/**
 * Initialize memory manager
 */
void init_memory_manager(MemoryManager *mm) {
    // Initialize contiguous memory as one large free block
    mm->memory_blocks = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    mm->memory_blocks->start_address = 0;
    mm->memory_blocks->size = MEMORY_SIZE;
    mm->memory_blocks->process_id = -1; // Free
    mm->memory_blocks->next = NULL;

    // Initialize page frames
    for (int i = 0; i < NUM_PAGES; i++) {
        mm->page_frames[i].process_id = -1;
        mm->page_frames[i].page_number = -1;
        mm->page_frames[i].last_access_time = 0;
    }

    // Initialize FIFO queue
    mm->fifo_front = 0;
    mm->fifo_rear = 0;
    mm->current_time = 0;
}

/**
 * Cleanup memory manager
 */
void cleanup_memory_manager(MemoryManager *mm) {
    MemoryBlock *current = mm->memory_blocks;
    while (current != NULL) {
        MemoryBlock *next = current->next;
        free(current);
        current = next;
    }
}

/**
 * Allocate contiguous memory using specified algorithm
 */
bool allocate_memory_contiguous(MemoryManager *mm, Process *process, AllocationAlgorithm algorithm) {
    MemoryBlock *best_block = NULL;
    MemoryBlock *current = mm->memory_blocks;

    // Find suitable block based on algorithm
    while (current != NULL) {
        if (current->process_id == -1 && current->size >= process->memory_size) {
            if (algorithm == FIRST_FIT) {
                best_block = current;
                break;
            } else if (algorithm == BEST_FIT) {
                if (best_block == NULL || current->size < best_block->size) {
                    best_block = current;
                }
            } else if (algorithm == WORST_FIT) {
                if (best_block == NULL || current->size > best_block->size) {
                    best_block = current;
                }
            }
        }
        current = current->next;
    }

    if (best_block == NULL) return false;

    // Allocate the block
    process->allocated_address = best_block->start_address;
    best_block->process_id = process->id;

    // Split block if larger than needed
    if (best_block->size > process->memory_size) {
        MemoryBlock *new_block = (MemoryBlock *)malloc(sizeof(MemoryBlock));
        new_block->start_address = best_block->start_address + process->memory_size;
        new_block->size = best_block->size - process->memory_size;
        new_block->process_id = -1; // Free
        new_block->next = best_block->next;
        
        best_block->size = process->memory_size;
        best_block->next = new_block;
    }

    return true;
}

/**
 * Deallocate contiguous memory
 */
void deallocate_memory_contiguous(MemoryManager *mm, Process *process) {
    if (process->allocated_address == -1) return;

    MemoryBlock *current = mm->memory_blocks;
    MemoryBlock *prev = NULL;

    // Find block to deallocate
    while (current != NULL) {
        if (current->start_address == process->allocated_address && current->process_id == process->id) {
            current->process_id = -1; // Mark as free
            
            // Merge with next block if free
            if (current->next != NULL && current->next->process_id == -1) {
                MemoryBlock *next_block = current->next;
                current->size += next_block->size;
                current->next = next_block->next;
                free(next_block);
            }
            
            // Merge with previous block if free
            if (prev != NULL && prev->process_id == -1) {
                prev->size += current->size;
                prev->next = current->next;
                free(current);
            }
            
            break;
        }
        prev = current;
        current = current->next;
    }

    process->allocated_address = -1;
}

/**
 * Allocate pages using specified page replacement algorithm
 */
bool allocate_pages(MemoryManager *mm, Process *process, PageReplacementAlgorithm algorithm) {
    int allocated_pages = 0;
    
    for (int page = 0; page < process->pages_needed; page++) {
        int free_frame = -1;
        
        // Find free frame
        for (int i = 0; i < NUM_PAGES; i++) {
            if (mm->page_frames[i].process_id == -1) {
                free_frame = i;
                break;
            }
        }
        
        // Use page replacement if no free frame
        if (free_frame == -1) {
            if (algorithm == FIFO) {
                free_frame = mm->fifo_queue[mm->fifo_front];
                mm->fifo_front = (mm->fifo_front + 1) % NUM_PAGES;
            } else if (algorithm == LRU) {
                free_frame = find_lru_page(mm);
            }
        }
        
        // Allocate frame
        mm->page_frames[free_frame].process_id = process->id;
        mm->page_frames[free_frame].page_number = page;
        mm->page_frames[free_frame].last_access_time = mm->current_time++;
        
        process->page_table[page] = free_frame;
        
        // Add to FIFO queue if new allocation
        if (algorithm == FIFO) {
            mm->fifo_queue[mm->fifo_rear] = free_frame;
            mm->fifo_rear = (mm->fifo_rear + 1) % NUM_PAGES;
        }
        
        allocated_pages++;
    }
    
    return allocated_pages == process->pages_needed;
}

/**
 * Deallocate pages
 */
void deallocate_pages(MemoryManager *mm, Process *process) {
    for (int page = 0; page < process->pages_needed; page++) {
        int frame = process->page_table[page];
        if (frame != -1) {
            mm->page_frames[frame].process_id = -1;
            mm->page_frames[frame].page_number = -1;
            mm->page_frames[frame].last_access_time = 0;
            process->page_table[page] = -1;
        }
    }
}

/**
 * Find LRU page for replacement
 */
int find_lru_page(MemoryManager *mm) {
    int lru_frame = 0;
    int min_time = mm->page_frames[0].last_access_time;
    
    for (int i = 1; i < NUM_PAGES; i++) {
        if (mm->page_frames[i].last_access_time < min_time) {
            min_time = mm->page_frames[i].last_access_time;
            lru_frame = i;
        }
    }
    
    return lru_frame;
}

/**
 * Access a page (update LRU time)
 */
void access_page(MemoryManager *mm, int frame_index) {
    if (frame_index >= 0 && frame_index < NUM_PAGES) {
        mm->page_frames[frame_index].last_access_time = mm->current_time++;
    }
}

/**
 * Print memory status
 */
void print_memory_status(MemoryManager *mm, Process processes[], int n) {
    printf("\n---Memory Management Status---\n");
    
    // Print contiguous memory allocation
    printf("\nContiguous Memory Allocation:\n");
    printf("Address\tSize\tProcess\tStatus\n");
    MemoryBlock *current = mm->memory_blocks;
    while (current != NULL) {
        printf("%d\t%d\t%s\t%s\n", 
               current->start_address, 
               current->size,
               current->process_id == -1 ? "N/A" : "",
               current->process_id == -1 ? "FREE" : "ALLOCATED");
        if (current->process_id != -1) {
            printf("\t\tP%d\t\n", current->process_id);
        }
        current = current->next;
    }
    
    // Print page allocation
    printf("\nPage Allocation:\n");
    printf("Frame\tProcess\tPage\tLast Access\n");
    for (int i = 0; i < NUM_PAGES; i++) {
        if (mm->page_frames[i].process_id != -1) {
            printf("%d\tP%d\t%d\t%d\n", 
                   i, 
                   mm->page_frames[i].process_id,
                   mm->page_frames[i].page_number,
                   mm->page_frames[i].last_access_time);
        }
    }
    
    // Print process memory information
    printf("\nProcess Memory Information:\n");
    printf("PID\tMemory Size\tPages Needed\tContiguous Addr\tPage Allocation\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t\t%d\t\t%d\t\t", 
               processes[i].id,
               processes[i].memory_size,
               processes[i].pages_needed,
               processes[i].allocated_address);
        
        for (int j = 0; j < processes[i].pages_needed; j++) {
            if (processes[i].page_table[j] != -1)
                printf("F%d ", processes[i].page_table[j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Main function
 */
int main() {
    Process processes[MAX_PROCESSES];
    GanttEntry gantt[MAX_GANTT_ENTRIES];
    MemoryManager mm;
    int n, gantt_count;

    // Initialize memory manager
    init_memory_manager(&mm);

    // Read process data
    n = read_processes_from_file("processes.txt", processes);
    if (n == 0) {
        cleanup_memory_manager(&mm);
        return 1;
    }

    printf("---Round Robin (RR) Scheduling with Memory Management (Time Quantum = %d)---\n", TIME_QUANTUM);
    printf("Memory Size: %d bytes, Page Size: %d bytes, Number of Pages: %d\n\n", 
           MEMORY_SIZE, PAGE_SIZE, NUM_PAGES);

    // Test different allocation algorithms
    printf("---Testing First-Fit Contiguous Allocation with FIFO Paging---\n");
    round_robin_with_memory(processes, n, gantt, &gantt_count, &mm, FIRST_FIT, FIFO);
    print_gantt_chart(gantt, gantt_count);
    print_scheduling_results(processes, n);
    print_memory_status(&mm, processes, n);

    // Reset for next test
    cleanup_memory_manager(&mm);
    init_memory_manager(&mm);
    
    // Reset process status
    for (int i = 0; i < n; i++) {
        processes[i].allocated_address = -1;
        processes[i].remaining_time = processes[i].burst_time;
        for (int j = 0; j < MAX_PROCESS_PAGES; j++) {
            processes[i].page_table[j] = -1;
        }
        processes[i].is_completed = false;
    }

    printf("\n---Testing Best-Fit Contiguous Allocation with LRU Paging---\n");
    round_robin_with_memory(processes, n, gantt, &gantt_count, &mm, BEST_FIT, LRU);
    print_gantt_chart(gantt, gantt_count);
    print_scheduling_results(processes, n);
    print_memory_status(&mm, processes, n);

    cleanup_memory_manager(&mm);
    return 0;
}