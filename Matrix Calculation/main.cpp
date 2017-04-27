/* 
Jose Hernandez

A program that will do a matrix calculate problem, while increasing its performance
by using threads.

I had issues with the unlock and lock variables, things were getting overwritten 
I had issues with the for loops for the X B C matrixes 
Slave function not returning correctly either 
 not working correctly 
*/
#include <sys/time.h> 
#include <pthread.h>
#include <cstdlib> 
#include <iostream>


using namespace std;

// time 
#define GET_TIME(now)\
{ struct timeval t; gettimeofday(&t, NULL);\
  now = t.tv_sec + t.tv_usec/1000000.0; }

int thread_count;
int a, b, c;
int** X;//global for slave function
int** B;//global for slave
int** C;//global for slave 


//thread to be able to unlock and lock
//initialize
pthread_mutex_t mu1;

// slave function prototype for threads
void *SlaveMatrix(void* rank);

// main
int main(int argc,char* argv[]) {
  double startTime, endTime, totalTime;
  // start timer 
  GET_TIME(startTime);
  // pass values from input 
  a = atoi(argv[1]);
  b = atoi(argv[2]);
  c = atoi(argv[3]);

//matrix for x
  X = new int*[a];
  for(int i = 0; i < a; i++) {
    X[i] = new int[b];

    for(int j = 0; j < b; j++) {
      X[i][j] = (i + j + 1); 
    }
  }

 //for B 
  B = new int*[b];
  for(int i = 0; i < b; i++) {
    B[i] = new int[c];

    for(int j = 0; j < c; j++) {
      B[i][j] = (i + j);
    }
  }

//for C
  C = new int*[a];
  for(int i = 0; i < a; i++) {
    C[i] = new int[c];
  }
//get how many threads and make an array 
  thread_count = atoi(argv[4]);
  pthread_t mt[thread_count];

  for(int thread_id = 0; thread_id < thread_count; thread_id++) {
    pthread_create(&mt[thread_id], NULL, SlaveMatrix, (void*)thread_id);//call slave 
  }

  for(int thread_id = 0; thread_id < thread_count; thread_id++) {
    pthread_join(mt[thread_id], NULL);
  }

  cout << "the first 20 and 10 multiplied are " << endl;
  for(int i = 0; i < 20; i++) {
    for(int j = 0; j < 10; j++) {
      cout << C[i][j] << " ";
    }
    cout << endl;
  }


  cout << "the last values from 20 and 10 multiplied togehter " << endl;
  for(int i = a-20; i < a; i++) {
    for(int j = c-10; j < c; j++) {
      cout << C[i][j] << " ";
    }
    cout << endl;
  }
  cout << "\n";
  cout << "\n";
  //stop timer 
  GET_TIME(endTime);

  // calculate total time 
  totalTime = endTime - startTime;
  cout << "Time elapsed: " << totalTime << endl;

  return 0;
}
//function definition
//not returining correctly here 
void *SlaveMatrix(void* rank) {
  // get thread rank
  //col and rows 
  int colA = 0;
  int colC = c;
  int rowB = 0;
  int rowC = a;
  //max rows and col 
  int colXMAX = b;
  int rowBMAX = b;
  int myRank = (long)rank;
  int row = myRank;
  int value = 0;
 //lock so cout wont get overwritten
  pthread_mutex_lock(&mu1);
  cout << "thread  " << myRank << ": " << row << "took approx" << a << " STEPS:" << thread_count << endl;
  //unlock 
  pthread_mutex_unlock(&mu1);

  while(row < rowC) {
    for(int i = 0; i < colC; i++) {
      colA = 0;
      rowB = 0;
      value = 0;
      while(colA < colXMAX && rowB < rowBMAX) {
        value += A[row][colA] * B[rowB][i];
        colA++;
        rowB++;
      }
      C[row][i] = value;
    }
    value += thread_count;
  }
}
