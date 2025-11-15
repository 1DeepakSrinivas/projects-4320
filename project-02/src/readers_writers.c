/**
 * Author: Deepak Govindarajan
 * CSC 4320 Operating Systems
 * Project 2: Readers-Writers Problem
 * 11/15/2025

 * Readers-Writers problem

 * The Readers-Writers problem is a classic synchronization problem where multiple readers and writers
 * access a shared resource. Readers can read concurrently, but writers have exclusive access.
 * Writers must block other readers while writing.

 * Input: processes.txt containing process information in the format: PID Arrival_Time Burst_Time Priority
 * Output: Output of the readers-writers problem with timestamps for each operation in the format: [Reader/Writer] [ID] [Operation] [Timestamp]
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "readers_writers.h"

// Global variables
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
static int r_count = 0;
static int w_waiting = 0;
static pthread_cond_t w_cond = PTHREAD_COND_INITIALIZER;

// Reader thread
void* reader(void* arg) {
    rw_thread_arg_t *args = (rw_thread_arg_t*)arg; // cast the argument to a rw_thread_arg_t pointer
    int id = args->id; // get the id of the reader
    
    for (int i = 0; i < 3; i++) {
        start_read();
        
        printf("[Reader %d] Reading\n", id); // print the reader id and reading message
        sleep(READING_TIME);
        printf("[Reader %d] Finished reading.\n", id); // print the reader id and finished reading message
        
        end_read();
        
        // pause before next read
        sleep(1);
    }
    
    return NULL;
}

// Writer thread function
void* writer(void* arg) {
    rw_thread_arg_t *args = (rw_thread_arg_t*)arg; // cast the argument to a rw_thread_arg_t pointer
    int id = args->id; // get the id of the writer
    
    for (int i = 0; i < 2; i++) { 
        start_write();
        
        printf("[Writer %d] Writing.\n", id);
        sleep(WRITING_TIME);
        printf("[Writer %d] Finished writing.\n", id);
        
        end_write();
        
        // pause before next write
        sleep(1);
    }
    
    return NULL;
}

// Start read
void start_read(void) {
    pthread_mutex_lock(&count_mutex);
    
    // Wait if writer is waiting to prevent writer starvation
    while (w_waiting > 0) {
        pthread_mutex_unlock(&count_mutex); // unlock the count mutex to allow other threads to access the resource
        usleep(1000); // sleep for 1ms to prevent busy waiting
        pthread_mutex_lock(&count_mutex); // lock the count mutex to prevent race conditions
    }
    
    r_count++;
    if (r_count == 1) {
        // First reader acquires the resource lock to prevent writer starvation
        pthread_mutex_lock(&mutex);
    }
    pthread_mutex_unlock(&count_mutex);
}

// End read
void end_read(void) {
    pthread_mutex_lock(&count_mutex); // lock the count mutex to prevent race conditions
    r_count--; // decrement the reader count
    if (r_count == 0) { // if the reader count is 0, then the last reader has finished reading
        pthread_mutex_unlock(&mutex); // unlock the resource lock to allow writers to write 
        pthread_cond_signal(&w_cond); // signal the writer condition variable to allow a writer to write
    }
    pthread_mutex_unlock(&count_mutex); // unlock the count mutex
}

// Start write
void start_write(void) {
    pthread_mutex_lock(&mutex);
    w_waiting++;
    
    // Wait until no readers are reading to prevent writer starvation
    while (r_count > 0) {
        pthread_cond_wait(&w_cond, &mutex);
    }
    
    w_waiting--; // decrement the writer waiting count
}

// End write
void end_write(void) {
    pthread_mutex_unlock(&mutex); // unlock the resource lock to allow readers to read
    pthread_cond_broadcast(&w_cond); // broadcast the writer condition variable to allow all writers to write
}

void readers_writers(void) {
    // Create reader and writer threads and store them in arrays
    pthread_t readers[NUM_READERS]; 
    pthread_t writers[NUM_WRITERS];
    
    rw_thread_arg_t reader_args[NUM_READERS];
    rw_thread_arg_t writer_args[NUM_WRITERS];
    
    // Create reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        reader_args[i].id = i; // set the id of the reader  
        reader_args[i].is_reader = 1; // set the reader flag to 1 (true)
        if (pthread_create(&readers[i], NULL, reader, &reader_args[i]) != 0) {
            perror("Reader thread creation failed"); 
            exit(1);
        }
    }
    
    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_args[i].id = i; // set the id of the writer
        writer_args[i].is_reader = 0; // set the reader flag to 0 (false)
        if (pthread_create(&writers[i], NULL, writer, &writer_args[i]) != 0) { 
            perror("Writer thread creation failed");
            exit(1);
        }
    }
    
    // Wait for all readers to complete
    for (int i = 0; i < NUM_READERS; i++) { 
        if (pthread_join(readers[i], NULL) != 0) { 
            perror("Reader thread join failed");
            exit(1);
        }
    }
    
    // Wait for all writers to complete 
    for (int i = 0; i < NUM_WRITERS; i++) {
        if (pthread_join(writers[i], NULL) != 0) { 
            perror("Writer thread join failed");
            exit(1);
        }
    }
    
    printf("Execution completed\n");
}

