/*
 Jose Hernandez 
 
Fix a poor synchronization output. The cout statements have many operations and those cout statements
from different threads interfere with each other. Making a cout statement
atomic and for that to happen we much use a mutex (a mutual exclusion) from pthreads. 
This will update the program with mutex variable, inittialize it and use it to protect cout statements
Also will get the time taken to do so

used cygwin to compile and run 
*/


#include <iostream>
//for threads for unlock and lock 
#include <pthread.h>  
#include <cstdlib> 
//for time 
#include <sys/time.h> 

using namespace std;

// macro for time 
#define GET_TIME(now)\

{ struct timeval t; gettimeofday(&t, NULL);\
  now = t.tv_sec + t.tv_usec/1000000.0; }

int thread_track;
//declare variale as mu1 
pthread_mutex_t mu1 = PTHREAD_MUTEX_INITIALIZER; 
//for large + numbers 
unsigned int globalSum; 
int const MAX = 500000000;
int *nums = new int[MAX]; 

//function prototype with one slave function
//this will be the slave function though
void *sumofall(void* rank);
void iNums();
 
int main(int argc, char* argv[])
{
    //double not int for realtime time
  double startTime, finishTime;
  // start the time 
  GET_TIME(startTime);
    //init the array 
  iNums(); 

  long thread_name; 
   //acii to integer 
  thread_track = atoi(argv[1]); 
  pthread_t myThreads[thread_track]; 
   //this is the part of the lab that shows how to make it atomic and use threads with a mutex
  //this is the creation of threads 
  for(thread_name = 0; thread_name < thread_track; thread_name++)
     pthread_create(&myThreads[thread_name], NULL, sumofall, (void*)thread_name);

  //wait for other processes then when you can, join 
  for(thread_name = 0; thread_name < thread_track; thread_name++)
     pthread_join(myThreads[thread_name], NULL);
  //stop timer and see time 
  GET_TIME(finishTime);

  cout << "\nOverall total sum: " << globalSum << endl; 
  //display 
  double totaltimetake = finishTime - startTime;
  cout << "Time elapsed:" << totaltimetake << endl;

  return 0;
}


//take the sum of all from part and rank 
void *sumofall(void* rank)
{
//we have to excluse so each can have its own and not access from any other 
pthread_mutex_lock(&mu1);  
int my_position = (long)rank; 

// calculate start and end index
unsigned int summparttt = 0;  
int part = MAX / thread_track;
  
  
int startIndex = part*my_position;
  
  
int endIndex = startIndex + part - 1;

  for(int i = startIndex; i <= endIndex; i++)
  {
    summparttt += nums[i];
  }
  cout << "\nThread ID:" << my_position << "\nstartIndex:" << startIndex << "\nendIndex:" << endIndex << "\nPartialSum:" << summparttt << endl;
  globalSum += summparttt;
  //unlock the pthread so others can access 
  pthread_mutex_unlock(&mu1);  
  return NULL;
}

//define initalize function here 
void iNums()
{
  for(long int i=0; i<MAX; i++)
  {
    nums[i] = i + 1;
  }
}
