// Primes.c

// threads-based program to find the number of primes between 2 and n;
// uses the Sieve of Eratosthenes, deleting all multiples of 2, all
// multiples of 3, all multiples of 5, etc. 

// Unix compilation:  gcc -g -o primes Primes.c -lpthread -lm

// usage:  primes n num_threads

#include <stdio.h>
#include <math.h>
#include <pthread.h>  // required for threads usage

#define MAX_N 1000 // Max 64 bit number
#define MAX_THREADS 25

//long long a= 4294967296;
// Show the value printf ( "% lld", a);


// shared variables; in threaded programs, all global variables are
// shared among all the threads; USE OF GLOBALS IS CENTRAL TO THREADED
// PROGRAMMING
int nthreads,  // number of threads (not counting main())
    n,  // range to check for primeness
    nextbase;  // next sieve multiplier to be used
    unsigned long long int prime[MAX_N+1];  // in the end, prime[i] = 1 if i prime, else 0
// lock for the shared variable nextbase
pthread_mutex_t nextbaselock = PTHREAD_MUTEX_INITIALIZER;
// ID structs for the threads
pthread_t id[MAX_THREADS];

// "crosses out" all odd multiples of k
void crossout(int k)
{  int i;
   for (i = 3; i*k <= n; i += 2)  {
      prime[i*k] = 0;
   }
}

// each thread runs this routine
void *worker(int tn)  // tn is the thread number (0,1,...)
{  
    int lim,base,work = 0;  // amount of work done by this thread
   // no need to check multipliers bigger than sqrt(n)
   lim = n;
   do  {
      // get next sieve multiplier
      // to avoid duplication across threads, change nextbase only with
      // the lock locked
      // lock the lock; if the lock is currently unlocked, this call
      // will return immediately and the lock state will be locked; if
      // on the other hand the lock is currently locked, this thread
      // will "block" (i.e. the call will NOT return) until the thread
      // that locked it does an unlock operation
      //printf("worker");
      pthread_mutex_lock(&nextbaselock);
      base = nextbase;
      nextbase += 1;
      // unlock the lock
      pthread_mutex_unlock(&nextbaselock);
      if (base <= lim)  {
         // don't bother crossing out if base known composite
         if (!prime[base])  {
            //crossout(base);
            if (n % base == 0){
                prime[base] = 1;
            }
            work++;  // log work done by this thread
         }
      }
      else {
          //printf("work - %i\n", work);
          return work; 
      }
   } while (1);
}

main(int argc, char **argv){  
    int nprimes,  // number of primes found 
    i,work;
    n = 15; // argument 1 number
    nthreads = 5;
    // mark all numbers as not factors until proven otherwise
    for (i = 0; i <= n; i++)  {
        prime[i] = 0;
    }
    nextbase = 2;

    // get threads started
    for (i = 0; i < nthreads; i++)  {
      pthread_create(&id[i],NULL,worker,i);
    }   
    
    

   // wait for all done
   for (i = 0; i < nthreads; i++)  {
      // this call says wait until thread number id[i] finishes execution, and assign the return value of that thread to our
      // local variable "work" here (to judge whether the work is evenly balanced among the threads)
      
      pthread_join(id[i],&work);
      printf("\n%d values of base done\n", work);
      for (int j =0; j<n;j++){
          printf("- %i - ", prime[j]);
      }
      
    //return 0;
   }
   
   // report results
   nprimes = 1;
   for (i = 0; i <= n; i++)
      if (prime[i])  {
         nprimes++;
      }
   printf("\nthe number of primes found was %d\n",nprimes);
}

/* 
debugging threaded programs under GDB (and the various GUIs that use 
GDB internally)

As you run a program under GDB, the creation of new threads will
be announced, e.g.

(gdb) r 100 2
Starting program: /debug/primes 100 2
[New Thread 16384 (LWP 28653)]
[New Thread 32769 (LWP 28676)]
[New Thread 16386 (LWP 28677)]
[New Thread 32771 (LWP 28678)]

You can do backtrace (bt) etc. as usual.  Here are some
threads-related commands:

   info threads (gives information on all current threads)

   thread 3 (change to thread 3)

   break 88 thread 3} (stop execution when thread 3 reaches
      source line 88)

   break 88 thread 3 if x==y} (stop execution when thread 3 reaches
      source line 88 and the variables x and y are equal)

Under the C shell, you can view all threads by typing

   ps -eLf
*/