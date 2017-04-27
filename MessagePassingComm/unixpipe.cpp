//Jose Hernandez
//This is a practice for parrellel programming with message passing communication
//among multiple processes using Unix pipe
//and to see which is faster recursion fibonacci or iterative fibonacci and getting 
//each of their time 
//compiled and ran using cygwin 
#include<unistd.h>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include <sys/wait.h> //for wait()
#include <sys/time.h>
using namespace std;
//macro (in-line expansion) for GET_TIME(double); needs <sys/time.h>
#define GET_TIME(now)\
{ struct timeval t; gettimeofday(&t, NULL);\
  now = t.tv_sec + t.tv_usec/1000000.0; }
#define MAX 100
//function prototypes
int Rfibonacci(int n);
int Ifbonacci(int n);
int main()
{
    int pid, status, i;
    int p1[2],p2[2],p3[2],p4[2]; //descriptors
    pipe(p1);
    pipe(p2);
    pipe(p3);
    pipe(p4);
    int response, response2, response3;
    double t1,t2;
    for(i=1; i<4; i++) // to create 3 childs
    {pid = fork(); //for each iteration in for loop create a fork
        if (pid == 0 && i == 1) //for the first child created "child process1"
            { 
                //display a menu and ask user to input a number from 1-100
                cout<<"Hi! \n";
                cout <<"Please choose a number from 1-100\n";
                cin >> response; // declared on top as int 
          
                //send a number N to both the second and third child processes, and displaying them
                
                write(p1[1], &response, sizeof(int));   // write from first child pipe, to child 2
                write(p4[1],&response,sizeof(int)); //write to child 3 from child 1
                //read time from recursive using pipeline
                read(p2[0],&t1,sizeof(double));
                //read time from iterative using pipeline
                read(p3[0],&t2,sizeof(double));
                cout << "Recursive time is "<< t1 << endl;
                cout << "Iterative time is "<< t2 << endl;
                exit(0);
            
            }
    
    else if (pid == 0 && i == 2) //creation of second child
            {
			double start, stop;
			GET_TIME(start);
            //second part of code 
            //read from child process 1 the number (response) 
            // getting info from child 1, and storing it in resposne2 by reference
            read(p1[0], &response2, sizeof(int)); 
            //execute recursive version of fibonacci function upon receiving N, N is the response
            int nacci = Rfibonacci(response2);
            //displaying the result
            cout << nacci << endl;
            //and returns execution time 
            GET_TIME(stop);
            double totalT = stop - start; 
            write (p2[1],&totalT, sizeof(double));
            exit(0); // so the second child exist
            }
            
    else if (pid == 0 && i == 3) // creation of the third child 
            {
			 double start, stop;
             //start time to get track
             GET_TIME(start);
             //read from pipe from child 1, and store as response3 by reference 
             read(p4[0], &response3, sizeof(int));
             //executuve iterative version of the fibonacci functions upon receiving number N aka response
             int inacci = Ifbonacci(response3);
             //displaying the result
             cout << inacci << endl;
             //stop timer
             GET_TIME(stop);
             //calculate total time 
             double endtime = stop - start;
             write(p3[1], &endtime, sizeof(double));
            
            exit(0);
            }
    
    }
    
    for (i =1; i <4; i++)
    {
        pid = wait(&status);
        cout << "Child (pid=" << pid <<")exited, status =" << status << endl;
    
    }


    return 0;
}
//define function for recursive fibonacci
 int Rfibonacci(int n)
            {
            if((n==1)||(n==0))
                {
                    return(n);
                }
            else
                {
                    return(Rfibonacci(n-1)+Rfibonacci(n-2));
                }
            }
            
//define function for iterative fibonacci
int Ifbonacci(int n)
{
    
    int temp= n;
    int tempY;
    int x = 0;
    int y = 0;
    
    while (n !=0)
    {
        if (temp == n)
            x = 1;
        else if (temp == n + 1)
            y = 1;
        else 
        {
            
            tempY = x;
            x = x+y;
            y = tempY;
            
        }
        n--;
        
    }
    
    
    return x;
}
