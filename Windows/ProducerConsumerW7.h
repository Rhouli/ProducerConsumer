/* Author: Ryan Houlihan
 */

#ifndef ProducerConsumer
#define ProducerConsumer

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFSIZE 15                            // Total Buffer Size
#define NUM_PRODUCER 1                         // Total producer threads
#define NUM_CONSUMER 1                         // Total consumer threads
#define TTL_THREADS (NUM_PRODUCER+NUM_CONSUMER)// Total threads

void producer(LPVOID lpParam);
void consumer(LPVOID lpParam);


#endif
