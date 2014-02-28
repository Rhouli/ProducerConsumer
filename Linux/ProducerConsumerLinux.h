/* Ryan Houlihan
 * Operating Systems CIS 3207
 * Lab 3: Linux producer-consumer problem
 * Header
 */

#ifndef ProducerConsumer
#define ProducerConsumer

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFFSIZE 15                            // Total Buffer Size
#define NUM_PRODUCER 1                         // Total producer threads
#define NUM_CONSUMER 1                         // Total consumer threads
#define TTL_THREADS (NUM_PRODUCER+NUM_CONSUMER)// Total threads
#define TRUE 0
#define FALSE 1

void *producer(void*);
void *consumer(void*);


#endif
