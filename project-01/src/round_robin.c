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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_PROCESSES 100
#define MAX_GANTT_ENTRIES 1000
#define TIME_QUANTUM 3

// Process struct
typedef struct
{
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;

    // Data calculated by round_robin
    int completion_time;
    int turnaround_time;
    int waiting_time;

    bool is_completed;
} Process;

// Gantt chart entry struct
typedef struct
{
    int process_id;
    int start_time;
    int end_time;
} GanttEntry;

// Function prototypes
int read_processes_from_file(const char *filename, Process processes[]);
void round_robin(Process processes[], int n, GanttEntry gantt[], int *gantt_count);
void print_gantt_chart(GanttEntry gantt[], int gantt_count);
void print_results(Process processes[], int n);

/**
 * Reads process data from a given file
 * Priority is ignored by the RR algorithm
 */

int read_processes_from_file(const char *filename, Process processes[])
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file '%s'\n", filename);
        return 0;
    }

    // Skip the header line of the file
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);

    int count = 0;
    int priority_; // A variable to read the priority value into, since we don't use it.

    // Read each process line from the file
    while (fscanf(file, "%d %d %d %d", &processes[count].id, &processes[count].arrival_time, &processes[count].burst_time, &priority_) == 4)
    {
        // Initialize the process status and remaining time
        processes[count].remaining_time = processes[count].burst_time;
        processes[count].is_completed = false;

        count++;
        if (count >= MAX_PROCESSES)
        {
            printf("Warning: Reached max process limit. Some processes may not have been read.\n");
            break;
        }
    }

    fclose(file);
    return count; // Return the total number of processes read
}

/**
 * Simulates the RR algorithm with time quantum, calculating completion, turnaround, and waiting times.
 * Also builds the Gantt chart for visualization.
 */
void round_robin(Process processes[], int n, GanttEntry gantt[], int *gantt_count)
{
    int current_time = 0;
    int completed_processes = 0;
    int queue[MAX_PROCESSES];
    int front = 0, rear = 0;
    bool in_queue[MAX_PROCESSES] = {false};
    *gantt_count = 0;

    // Add processes that arrive at time 0 to the queue
    for (int i = 0; i < n; i++)
    {
        if (processes[i].arrival_time == 0)
        {
            queue[rear++] = i;
            in_queue[i] = true;
        }
    }

    // Main scheduling loop
    while (completed_processes < n)
    {
        // If queue is empty, advance time to next arrival
        if (front == rear)
        {
            int next_arrival = -1;
            for (int i = 0; i < n; i++)
            {
                if (!processes[i].is_completed && processes[i].arrival_time > current_time)
                {
                    if (next_arrival == -1 || processes[i].arrival_time < processes[next_arrival].arrival_time)
                    {
                        next_arrival = i;
                    }
                }
            }

            if (next_arrival != -1)
            {
                current_time = processes[next_arrival].arrival_time;
                queue[rear++] = next_arrival;
                in_queue[next_arrival] = true;
            }
            continue;
        }

        // Get the next process from the queue
        int current_process = queue[front++];
        Process *p = &processes[current_process];

        // Calculate execution time for this quantum
        int execution_time = (p->remaining_time > TIME_QUANTUM) ? TIME_QUANTUM : p->remaining_time;

        // Record Gantt chart entry
        gantt[*gantt_count].process_id = p->id;
        gantt[*gantt_count].start_time = current_time;
        gantt[*gantt_count].end_time = current_time + execution_time;
        (*gantt_count)++;

        // Execute the process for the calculated time
        current_time += execution_time;
        p->remaining_time -= execution_time;

        // Check for newly arrived processes during execution
        for (int i = 0; i < n; i++)
        {
            if (!processes[i].is_completed && !in_queue[i] &&
                processes[i].arrival_time <= current_time && processes[i].arrival_time > (current_time - execution_time))
            {
                queue[rear++] = i;
                in_queue[i] = true;
            }
        }

        // Check if process is completed
        if (p->remaining_time == 0)
        {
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            p->is_completed = true;
            in_queue[current_process] = false;
            completed_processes++;
        }
        else
        {
            // Add process back to the queue if not completed
            queue[rear++] = current_process;
        }
    }
}

/**
 * Prints the Gantt chart showing process execution order
 */
void print_gantt_chart(GanttEntry gantt[], int gantt_count)
{
    printf("\nGantt Chart:\n");

    // Print the process names
    printf("|");
    for (int i = 0; i < gantt_count; i++)
    {
        printf(" P%d |", gantt[i].process_id);
    }
    printf("\n");

    // Print the time line
    printf("%d", gantt[0].start_time);
    for (int i = 0; i < gantt_count; i++)
    {
        printf("    %d", gantt[i].end_time);
    }
    printf("\n\n");
}

/**
 * Prints the final results, including a table and average times.
 */
void print_results(Process processes[], int n)
{
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    printf("Final Process Statistics:\n");
    printf("PID\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time\n");

    for (int i = 0; i < n; i++)
    {
        printf("%d\t%d\t\t%d\t\t%d\t\t%d\n",
               processes[i].id,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].waiting_time,
               processes[i].turnaround_time);

        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }
    printf("\n");

    printf("Average Waiting Time:    %.2f\n", (double)total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", (double)total_turnaround_time / n);
}

int main()
{
    Process processes[MAX_PROCESSES];
    GanttEntry gantt[MAX_GANTT_ENTRIES];
    int n;           // This will store the actual number of processes read from the file.
    int gantt_count; // Number of Gantt chart entries

    // Read the process data from the file
    n = read_processes_from_file("processes.txt", processes);

    // If no processes were read, exit the program.
    if (n == 0)
    {
        return 1;
    }

    printf("--- Round Robin (RR) Scheduling (Time Quantum = %d) ---\n", TIME_QUANTUM);

    rr_(processes, n, gantt, &gantt_count);

    print_gantt_chart(gantt, gantt_count);
    print_results(processes, n);

    return 0;
}