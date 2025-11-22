#ifndef READERS_WRITERS_H
#define READERS_WRITERS_H

#include <pthread.h>

// Constants for readers and writers
#define NUM_READERS 5
#define NUM_WRITERS 3
#define READING_TIME 2
#define WRITING_TIME 3

// Thread argument structure for readers and writers
typedef struct {
    int id;
    int is_reader;
} rw_thread_arg_t;

// Readers-Writers functions prototypes
void* reader(void* arg);
void* writer(void* arg);
void start_read(void);
void end_read(void);
void start_write(void);
void end_write(void);
void readers_writers(void);

#endif /* READERS_WRITERS_H */

