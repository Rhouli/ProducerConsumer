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
 * lock it "comsumes" (reads)  an item from the buffer. It then unlocks the mutex and 
 * signals the empty semaphor.
 */

#include "ProducerConsumerW7.h"

// Global Variables
int conPos = 0, proPos = 0;
int *buffer;
HANDLE full, empty;
static int INDEX = 40; // total number of items to be produced and consumed
HANDLE mutex;
FILE* logFile;

int main(void){
   
  int x;
  // Create TTL_THREADS threads
  HANDLE threads[TTL_THREADS];
  DWORD ThreadID;
  // Open file
  logFile = fopen("./logWindows.txt", "wt");
  // Initalize buffer
  buffer = (int *) malloc(sizeof(int)*BUFFSIZE);

  // initalize mutex and semaphor variable objects
  mutex = CreateMutex(NULL, FALSE, NULL);
  if(mutex==NULL){
    fprintf(stderr,"CreateMutex error: %d\n", GetLastError());
    return 1;
  }
  full = CreateSemaphore(NULL, 0, BUFFSIZE, NULL);
  if(full == NULL){
    fprintf(stderr,"CreateSemaphore error: %d\n", GetLastError());
    return 1;
  }
  empty = CreateSemaphore(NULL, BUFFSIZE, BUFFSIZE, NULL);
  if(empty == NULL){
    fprintf(stderr,"CreateSemaphore error: %d\n", GetLastError());
    return 1;
  }  
  // Create Producer thread(s)    
  for(x = 0; x < NUM_CONSUMER; x++){
    threads[x] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) producer, NULL, 0, &ThreadID);
    if(threads[x] == NULL){
      fprintf(stderr,"CreateThread error: %d\n", GetLastError());
      return 1;
    }
  }
  // Create Consumer thread(s)
  for( ; x < TTL_THREADS; x++){
    threads[x] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) consumer, NULL, 0, &ThreadID);
    if(threads[x] == NULL){
      fprintf(stderr,"CreateThread error: %d\n", GetLastError());
      return 1;
    }
  }

  // Wait for all threads
  WaitForMultipleObjects(TTL_THREADS, threads, TRUE, INFINITE);

  // Print to logFile
  fprintf(logFile, "Threads have completed\n");
  for(x = 0; x < BUFFSIZE; x++)
    fprintf(logFile, "Buffer[%ld] is __** %4d **__\n", x, buffer[x]);
  fclose(logFile);
  
// Clean and exit
  for(x=0; x < TTL_THREADS; x++)
    CloseHandle(threads[x]);
  CloseHandle(mutex);
  CloseHandle(full);
  CloseHandle(empty);

  return 0;
}

void producer(LPVOID lpParam){

  int product, i;
  UNREFERENCED_PARAMETER(lpParam);
  
  // for maxnumber of items to produce
  for(i = 0; i < INDEX; i++){ 
     Sleep(rand()%4);
     // genereate random product 
     product = rand()%100 + 1;
     // get a lock on the semaphor
     WaitForSingleObject(empty, INFINITE);
     // acquire next lock
     WaitForSingleObject(mutex, INFINITE);
     // if full wait for consumer
     buffer[proPos%BUFFSIZE] = product;
     proPos++;
     // print to log
     fprintf(logFile,"Produced %d <==\n", product);
     // unlock mutex and signal full
     ReleaseMutex(mutex);
     ReleaseSemaphore(full, 1, NULL);
  }
  return;
}

void consumer(LPVOID lpParam){
	
  int consumed, i;
  UNREFERENCED_PARAMETER(lpParam);
  
  // for max number of items to consume
  for(i = 0; i < INDEX; i++){
    Sleep(rand()%6);
    // lock semaphor
    WaitForSingleObject(full, INFINITE);
    // lock the mutex
    WaitForSingleObject(mutex, INFINITE);
    // consume an item
    consumed = buffer[conPos%BUFFSIZE];
    conPos++;
    // print to log
    fprintf(logFile,"Consumed %d ==>\n", consumed);
    // unlock mutex and signal empty
    ReleaseMutex(mutex);
    ReleaseSemaphore(empty, 1, NULL);
  }
  return;
}


