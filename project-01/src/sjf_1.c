/**
 * Author: Deepak Govindarajan
 * CSC 4320 Operating Systems
 * Project 1: Process Scheduling Simulation
 * 9/25/2025

 * Shortest Job First (SJF) scheduling algorithm

 * The SJF algorithm selects the process with the shortest burst time to run next.
 * This is a non-preemptive algorithm where once a process starts execution, it runs to completion.

 * Input: processes.txt containing process information in format: process_id arrival_time burst_time
 * Output: Execution sequence, waiting time, and turnaround time for each process
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_PROCESSES 100
#define MAX_GANTT_ENTRIES 200

// Process struct
typedef struct
{
    int id;
    int arrival_time;
    int burst_time;

    // Data calculated by sjf_
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
void sjf_(Process processes[], int n, GanttEntry gantt[], int *gantt_count);
void gantt_chart(GanttEntry gantt[], int gantt_count);
void print_results(Process processes[], int n);

/**
 * Reads process data from a given file
 * Priority is ignored by the SJF algorithm
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
    int priority_; // Read but not used in SJF

    // Read each process line from the file
    while (fscanf(file, "%d %d %d %d", &processes[count].id, &processes[count].arrival_time, &processes[count].burst_time, &priority_) == 4)
    {

        // Initialize the process status
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
 * Simulates the SJF algorithm, calculating completion, turnaround, and waiting times.
 * Also builds the Gantt chart for visualization.
 */

void sjf_(Process processes[], int n, GanttEntry gantt[], int *gantt_count)
{
    if (n <= 0)
        return; // No processes to schedule
    int current_time = 0;
    int completed_processes = 0;
    *gantt_count = 0;

    // Schedule processes until all are done
    while (completed_processes < n)
    {
        int shortest_job_index = -1;
        int shortest_burst = INT_MAX;

        // Find shortest job that's ready to run
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && !processes[i].is_completed)
            {
                // Select shorter burst time, break ties by arrival time
                if (processes[i].burst_time < shortest_burst ||
                    (processes[i].burst_time == shortest_burst &&
                     processes[i].arrival_time < processes[shortest_job_index].arrival_time))
                {
                    shortest_burst = processes[i].burst_time;
                    shortest_job_index = i;
                }
            }
        }

        // Execute the selected process
        if (shortest_job_index != -1)
        {
            Process *p = &processes[shortest_job_index];

            // Record Gantt chart entry
            gantt[*gantt_count].process_id = p->id;
            gantt[*gantt_count].start_time = current_time;
            gantt[*gantt_count].end_time = current_time + p->burst_time;
            (*gantt_count)++;

            // Advance time by the burst time of the chosen process.
            current_time += p->burst_time;

            // Calculate final metrics
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;

            p->is_completed = true;
            completed_processes++;
        }
        else
        {
            // Find the earliest arrival time of unstarted processes
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++)
            {
                if (!processes[i].is_completed && processes[i].arrival_time > current_time)
                {
                    if (processes[i].arrival_time < next_arrival)
                        next_arrival = processes[i].arrival_time;
                }
            }

            if (next_arrival != INT_MAX)
                current_time = next_arrival;
            else
                current_time++; // Fallback
        }
    }
}

// Prints the Gantt chart showing process execution order

void gantt_chart(GanttEntry gantt[], int gantt_count)
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

// Prints the final results, including a table and average times.

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
    int n;
    int gantt_count;

    // Read the process data from the file
    n = read_processes_from_file("processes.txt", processes);

    // If no processes were read, exit the program.
    if (n == 0)
    {
        return 1;
    }

    printf("--- Shortest Job First (SJF) Scheduling ---\n");

    sjf_(processes, n, gantt, &gantt_count);

    gantt_chart(gantt, gantt_count);
    print_results(processes, n);

    return 0;
}