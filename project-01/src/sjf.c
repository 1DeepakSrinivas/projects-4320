/**
 *
 * Author: Deepak Govindarajan
 * CSC 4320 Operating Systems
 * Project 1: Process Scheduling Simulation
 * 9/23/2025
 *
 * Shortest Job First (SJF) scheduling algorithm
 *
 * The SJF algorithm selects the process with the shortest burst time to run next.
 * This is a non-preemptive algorithm where once a process starts execution, it runs to completion.
 *
 * Input: processes.txt containing process information in format: process_id arrival_time burst_time
 * Output: Execution sequence, waiting time, and turnaround time for each process
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_PROCESSES 100

// Struct to store process information
typedef struct
{
    int id;
    int arrival_time;
    int burst_time;
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    bool executed;
} Process;

// Function to read process information from file
int readProcesses(const char *filename, Process processes[])
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        return 0;
    }

    // Skip the header line
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);

    int count = 0;
    int priority; // Variable to read and ignore the priority column

    // Read process data until EOF or max processes reached
    while (fscanf(file, "%d %d %d %d", &processes[count].id,
                  &processes[count].arrival_time, &processes[count].burst_time, &priority) == 4)
    {
        processes[count].executed = false;
        count++;
        if (count >= MAX_PROCESSES)
            break;
    }

    fclose(file);
    return count;
}

// Sorting processes based on burst time that have not been executed
int findNextProcess(Process processes[], int n, int current_time)
{
    int shortest_job_index = -1;
    int shortest_time = INT_MAX;

    // Iterate through all processes to find the next process to execute
    for (int i = 0; i < n; i++)
    {
        if (!processes[i].executed && processes[i].arrival_time <= current_time &&
            processes[i].burst_time < shortest_time)
        {
            shortest_time = processes[i].burst_time;
            shortest_job_index = i;
        }
    }

    return shortest_job_index;
}

// SJF Implementation
void sjfScheduling(Process processes[], int n)
{
    int completed = 0;
    int current_time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    printf("\nExecution Sequence:\n");

    // Process until all processes are executed
    while (completed < n)
    {
        // Find process with shortest burst time that has arrived
        int next_process = findNextProcess(processes, n, current_time);

        // If no process is available at current time
        if (next_process == -1)
        {
            // Find the earliest arriving process that hasn't been executed yet
            int earliest_arrival = INT_MAX;
            int earliest_index = -1;

            for (int i = 0; i < n; i++)
            {
                if (!processes[i].executed && processes[i].arrival_time < earliest_arrival)
                {
                    earliest_arrival = processes[i].arrival_time;
                    earliest_index = i;
                }
            }

            // Jump to the arrival time of the next process
            if (earliest_index != -1)
            {
                current_time = processes[earliest_index].arrival_time;
                continue;
            }
            else
            {
                // All processes have been considered
                break;
            }
        }

        // Record start time of the process
        processes[next_process].start_time = current_time;

        // Execute the process (update current time)
        current_time += processes[next_process].burst_time;

        // Record completion time
        processes[next_process].completion_time = current_time;

        // Calculate turnaround time (completion time - arrival time)
        processes[next_process].turnaround_time = processes[next_process].completion_time -
                                                  processes[next_process].arrival_time;

        // Calculate waiting time (turnaround time - burst time)
        processes[next_process].waiting_time = processes[next_process].turnaround_time -
                                               processes[next_process].burst_time;

        // Update totals
        total_waiting_time += processes[next_process].waiting_time;
        total_turnaround_time += processes[next_process].turnaround_time;

        // Mark as executed
        processes[next_process].executed = true;
        completed++;

        // Print execution information
        printf("Time %d: Process %d starts execution\n",
               processes[next_process].start_time, processes[next_process].id);
        printf("Time %d: Process %d completes execution\n",
               processes[next_process].completion_time, processes[next_process].id);
    }

    // Print process statistics
    printf("\nProcess Statistics:\n\n");
    printf("PID\tArrival\tBurst\tStart\tCompletion\tWaiting\tTurnaround\n");

    for (int i = 0; i < n; i++)
    {
        printf("%d\t%d\t%d\t%d\t%d\t\t%d\t%d\n",
               processes[i].id, processes[i].arrival_time, processes[i].burst_time,
               processes[i].start_time, processes[i].completion_time,
               processes[i].waiting_time, processes[i].turnaround_time);
    }

    // Calculate and print averages
    double avg_waiting_time = (double)total_waiting_time / n;
    double avg_turnaround_time = (double)total_turnaround_time / n;

    printf("\nAverage Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
}

int main()
{
    Process processes[MAX_PROCESSES];
    int n;

    // Read process information from file
    n = readProcesses("processes.txt", processes);

    if (n == 0)
    {
        printf("No processes to schedule.\n");
        return 1;
    }

    printf("---Shortest Job First (SJF) Scheduling---\n");

    sjfScheduling(processes, n);

    return 0;
}