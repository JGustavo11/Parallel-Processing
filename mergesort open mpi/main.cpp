/*
Jose Hernandez

This is a parallel merge sort with open mpi
There is a size n, that for each thread, its
assigned to a local list of n/p elements. However, in this program the thread will perform something
called qsort to sort the local list instead of using a recursion method. Its done merging step
like a tree reduction. So you essentially have the sorted thread 1 combine with the sorted thread 2
, then the sorted thread 2 combines with the sorted thread 3 and so forth.


ran and compiled on cygwin
make sure to have the terminal set to correct directory on main.cpp you are going to compile

To compile its g++ -fopenmp main.cpp --creates a.exe

first run is ./a 100 4
second run is ./a 100000000 1 2 4 8

*/
#include <time.h>

//to be able to use openmp
#include <omp.h>

#include <iostream>

//essential for qsort
#include <stdlib.h>

#include <cstdlib>
using namespace std;

// function prototypes

void watchSort(int* allOfList, int n);
void sortlocally(int* allOfList, int n, int p);
int compare (const void * a, const void * b);
void combine(int n, int p, int receive, int send, int sizeofList,int* allOfList, int cDifference, int divisor);


int main(int argc,char* argv[]) {
//get the length of n
int n = atoi(argv[1]);
//then get how many threads
int p = atoi(argv[2]);
int remaining = 0;

//set time to start time and end time to calcualte time
double startTime = 0;
double endTime = 0;
int* listNum = new int[n];
//from 1 to how ever many n they entered above
  srand(time(NULL));
  #pragma parallel for
    for(int i = 0; i < n; i++) {
        listNum[i] = rand() % n + 1;
    }
//start time with omp built in
startTime = omp_get_wtime();
  #pragma omp parallel num_threads(p)
    sortlocally(listNum,n,p);

  for(int j = 0; j < n; j++) {
    cout << listNum[j] << " ";
  }
  endTime = omp_get_wtime();
  cout << "TIME:" << endTime - startTime << endl;
  watchSort(listNum, n);
  return 0;
}
//defining functions
void combine(int receive, int send, int sizeofList, int cDifference, int divisor, int* allOfList, int n, int p) {

  int startWhenReceive = receive * sizeofList;
  int startwhenSend = send * sizeofList;
  int size = sizeofList*divisor;
  int endWhenReeive = startWhenReceive + (sizeofList * cDifference);
  int endWhenSend = startwhenSend + (sizeofList * cDifference);

  if(send == (p - cDifference) && (n%p) != 0) {
    endWhenSend += n%p;
    size += n%p;
  }

  int* localList = new int[size];
  int i = 0;

  int tempRecStart = startWhenReceive;
  int tempSendStart = startwhenSend;

  while(tempRecStart != endWhenReeive && tempSendStart != endWhenSend) {
    if(allOfList[tempRecStart] <= allOfList[tempSendStart]) {
      localList[i] = allOfList[tempRecStart];
      i++;
      tempRecStart++;
    }
    else if(allOfList[tempRecStart] >= allOfList[tempSendStart]) {
      localList[i] = allOfList[tempSendStart];
      i++;
      tempSendStart++;
    }
  }

  if(tempRecStart == endWhenReeive && tempSendStart != endWhenSend) {
    for(tempSendStart; tempSendStart < endWhenSend; tempSendStart++, i++) {
      localList[i] = allOfList[tempSendStart];
    }
  }
  else if(tempSendStart == endWhenSend && tempRecStart != endWhenReeive) {
    for(tempRecStart; tempRecStart < endWhenReeive; tempRecStart++, i++) {
      localList[i] = allOfList[tempRecStart];
    }
  }


  i = 0;
  #pragma parallel for
    for(startWhenReceive; startWhenReceive < endWhenSend; startWhenReceive++, i++) {
      allOfList[startWhenReceive] = localList[i];
  }
}



void watchSort(int* allOfList, int n) {
  for(int i = 0; i < n - 1; i++) {
    if(allOfList[i] > allOfList[i+1]) {
      cout << "not sorted " << endl;
      return;
    }
  }
  cout << "SORTED" << endl;
}


void sortlocally(int* allOfList, int n, int p) {
    //get the rank using omp
  int my_rank = omp_get_thread_num();
  int sizeofList, my_start, my_end;
  bool remainFlag = false;

  sizeofList = n/p;
  my_start = sizeofList*my_rank;

  int* localList = new int[sizeofList];
  //end globall
  my_end = my_start + sizeofList;

  int i = 0;
  int tempStart = my_start;

  // put values of  localList from allOfList
  for(tempStart; tempStart < my_end; tempStart++, i++) {
    localList[i] = allOfList[tempStart];
  }

  //qsort
  qsort(localList, sizeofList, sizeof(int), compare);

  // update allOfList with localList
  i = 0;
  #pragma parallel for
    for (my_start; my_start < my_end; my_start++, i++) {
    allOfList[my_start] = localList[i];
  }

  int divisor = 2;
  int coreDifference = 1;

  while(divisor <= p) {
    #pragma omp barrier
    if(my_rank % divisor == 0) {
      // received and call combine function
      combine(my_rank, my_rank + coreDifference, sizeofList, coreDifference, divisor, allOfList, n, p);
    }
    else {

    }
    divisor *= 2;
    coreDifference *= 2;
  }
}
int compare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}
