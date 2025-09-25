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

// Function prototypes
int read_processes_from_file(const char *filename, Process processes[]);
void sjf_(Process processes[], int n);
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
    int priority_; // A variable to read the priority value into, since we don't use it.

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
 */

void sjf_(Process processes[], int n)
{
    int current_time = 0;
    int completed_processes = 0;

    // The main loop continues until all processes are completed.
    while (completed_processes < n)
    {
        int shortest_job_index = -1;
        int shortest_burst = INT_MAX;

        // Find the uncompleted process that has arrived and has the shortest burst time.
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival_time <= current_time && !processes[i].is_completed)
            {
                if (processes[i].burst_time < shortest_burst)
                {
                    shortest_burst = processes[i].burst_time;
                    shortest_job_index = i;
                }
            }
        }

        // If a suitable process was found, execute it.
        if (shortest_job_index != -1)
        {
            Process *p = &processes[shortest_job_index];

            // Advance time by the burst time of the chosen process.
            current_time += p->burst_time;

            // Calculate metrics for the completed process.
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;

            p->is_completed = true;
            completed_processes++;
        }
        else
        {
            // If no process has arrived yet, the CPU is idle.
            // Advancing the clock by one time unit.
            current_time++;
        }
    }
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
    int n; // This will store the actual number of processes read from the file.

    // Read the process data from the file
    n = read_processes_from_file("processes.txt", processes);

    // If no processes were read, exit the program.
    if (n == 0)
    {
        return 1;
    }

    printf("--- Shortest Job First (SJF) Scheduling ---\n");

    sjf_(processes, n);

    print_results(processes, n);

    return 0;
}