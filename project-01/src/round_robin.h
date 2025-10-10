#ifndef ROUND_ROBIN_H
#define ROUND_ROBIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Constants for Round Robin scheduling
#define MAX_PROCESSES 100
#define MAX_GANTT_ENTRIES 1000
#define TIME_QUANTUM 3
#define MEMORY_SIZE 1024
#define PAGE_SIZE 64
#define NUM_PAGES (MEMORY_SIZE / PAGE_SIZE)
#define MAX_PROCESS_PAGES 8

// Process structure for Round Robin
typedef struct {
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    bool is_completed;
    
    // Memory management fields
    int memory_size;
    int allocated_address;
    int pages_needed;
    int page_table[MAX_PROCESS_PAGES];
} Process;

// Gantt chart entry
typedef struct {
    int process_id;
    int start_time;
    int end_time;
} GanttEntry;

// Memory block for contiguous allocation
typedef struct MemoryBlock {
    int start_address;
    int size;
    int process_id;
    struct MemoryBlock *next;
} MemoryBlock;

// Page frame structure
typedef struct {
    int process_id;
    int page_number;
    int last_access_time;
} PageFrame;

// Memory management system
typedef struct {
    MemoryBlock *memory_blocks;
    PageFrame page_frames[NUM_PAGES];
    int fifo_queue[NUM_PAGES];
    int fifo_front, fifo_rear;
    int current_time;
} MemoryManager;

// Allocation algorithms
typedef enum {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
} AllocationAlgorithm;

// Page replacement algorithms
typedef enum {
    FIFO,
    LRU
} PageReplacementAlgorithm;

// Function declarations
int read_processes_from_file(const char *filename, Process processes[]);
void round_robin_schedule(Process processes[], int n, GanttEntry gantt[], int *gantt_count);
void round_robin_with_memory(Process processes[], int n, GanttEntry gantt[], int *gantt_count, 
                             MemoryManager *mm, AllocationAlgorithm alloc_alg, PageReplacementAlgorithm page_alg);
void print_gantt_chart(GanttEntry gantt[], int gantt_count);
void print_scheduling_results(Process processes[], int n);

// Memory management functions
void init_memory_manager(MemoryManager *mm);
void cleanup_memory_manager(MemoryManager *mm);
bool allocate_memory_contiguous(MemoryManager *mm, Process *process, AllocationAlgorithm algorithm);
void deallocate_memory_contiguous(MemoryManager *mm, Process *process);
bool allocate_pages(MemoryManager *mm, Process *process, PageReplacementAlgorithm algorithm);
void deallocate_pages(MemoryManager *mm, Process *process);
void print_memory_status(MemoryManager *mm, Process processes[], int n);
int find_lru_page(MemoryManager *mm);
void access_page(MemoryManager *mm, int frame_index);

#endif // ROUND_ROBIN_H
