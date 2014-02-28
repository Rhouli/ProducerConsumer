/* Author: Ryan Houlihan
 *
 * Main:
 * Creates NUM_PRODUCER producer threads, NUM_COMSUMER consumer threads, with a 
 * buffer of size BUFFSIZE. Creates all threads, a mutex, two sempahors, one for 
 * the empty status, one for the full status. Then it joins the threads and waits
 * for them to finish producing and consuming before exiting.
 *
 * Producer:
 * Sleeps randomly to show that there truly is mutual exclusion. 
 * Produces a random integer as a product. Checks to make sure the buffer is not
 * full and then it trys to acquire a lock on the mutex and is blocked if no lock is
 * avaliable. One the lock is freed it acquires the lock and enters into the critical 
 * section. It adds the product to the buffer and then releases the lock and sents a signal
 * to the full semaphore.
 * 
 * Consumer:
 * Sleeps randomly to show that there truly is mutual exclusion. 
 * Makes sure buffer is not empty, if it is it blocks. Once the buffer is not empty it
 * attempts to acquire a lock on the mutex. Once the mutex is unlocked and it acquires a
 * lock it "consumes" (reads)  an item from the buffer. It then unlocks the mutex and 
 * signals the empty semaphor.
 */

#include "ProducerConsumerLinux.h"

// Global Variables
int conPos = 0, proPos = 0;
int *buffer;
sem_t full, empty;
static int INDEX = 85; // total number of items to be produced and consumed
pthread_cond_t consumer_cond;
pthread_mutex_t count_mutex;
FILE* logFile;

int main(void){
  // Create TTL_THREADS threads
  pthread_t threads[TTL_THREADS];
  pthread_attr_t attr;
  // Open file
  logFile = fopen("./logLinux.txt", "wt");
  // Initalize buffer
  buffer = (int *) malloc(sizeof(int)*BUFFSIZE);

  // initalize mutex and semaphor variable objects
  pthread_mutex_init(&count_mutex, NULL);
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, BUFFSIZE);

  // Create threads in joinable state
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  // Create Producer thread(s)    
  long x;
  for(x = 0; x < NUM_CONSUMER; x++)
   pthread_create(&threads[x], &attr, producer, (void *)x);  
  // Create Consumer thread(s)
  for( ; x < TTL_THREADS; x++)
    pthread_create(&threads[x], &attr, consumer, (void *)x);

  // Wait for all threads
  for(x=0; x < TTL_THREADS; x++)
    pthread_join(threads[x], NULL);		 

  fprintf(logFile, "Threads have completed\n");
  for(x = 0; x < BUFFSIZE; x++)
    fprintf(logFile, "Buffer[%ld] is __** %4d **__\n", x, buffer[x]);
  fclose(logFile);

  // Clean and exit
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&count_mutex);
  sem_destroy(&full);
  sem_destroy(&empty);
  pthread_exit(NULL);
}
void *producer(void* t){
  int product, i;
  
  // for maxnumber of items to produce
  for(i = 0; i < INDEX; i++){ 
    sleep((rand()%1)/100);
     // genereate random product 
     product = rand()%100 + 1;
     // get a lock on the semaphor
     sem_wait(&empty);
     // acquire next lock
     pthread_mutex_lock(&count_mutex);
     // if full wait for consumer
     buffer[proPos%BUFFSIZE] = product;
     proPos++;
     // print to log
     fprintf(logFile,"Produced %d <==\n", product);
     // unlock mutex and signal full
     pthread_mutex_unlock(&count_mutex);
     sem_post(&full);
  }
  pthread_exit(NULL);
}

void *consumer(void* t){
  int consumed, i;
  
  // for max number of items to consume
  for(i = 0; i < INDEX; i++){
    sleep(rand()%1);
    // lock semaphor
    sem_wait(&full);
    // lock the mutex
    pthread_mutex_lock(&count_mutex);
    // consume an item
    consumed = buffer[conPos%BUFFSIZE];
    conPos++;
    // print to log
    fprintf(logFile,"Consumed %d ==>\n", consumed);
    // unlock mutex and signal empty
    pthread_mutex_unlock(&count_mutex);
    sem_post(&empty);
  }
  pthread_exit(NULL);
}




