#include <stdio.h>
#include <math.h>
#include <pthread.h>  // required for threads usage

#define MAX_N 100000000
#define MAX_THREADS 320
#define INT_BITS 32


int rightRotate(int n, unsigned int d){ 
   /* In n>>d, first d bits are 0. To put last 3 bits of at  
     first, do bitwise or of n>>d with n <<(INT_BITS - d) */
   return (n >> d)|(n << (INT_BITS - d)); 
} 

int trialDivision(int n){
    int f = 1;
    int ctr = 0;
    while(n >= f){
        if (n % f == 0){
            ctr++;
        } 
        f++;
    }
    return ctr;
}


// ID structs for the threads
pthread_t id[MAX_THREADS];

struct args {
    int tid;
    int input;
};

// each thread runs this routine
void *worker(void *data)  
{  int work = 0;
   int threadId = ((struct args*)data)-> tid;
   int threadInput = ((struct args*)data)-> input;
   //printf("id:%d, input:%d\n", threadId, threadInput);
   work =  trialDivision(threadInput);
   sleep(1);
   free(data);
    return work;
   //return 1;
}

int main(){  
    int work;
    int n = 12345;
    int nthreads = 32;
    

   // get threads started
    int f;
    for (int i = 0; i < nthreads; i++)  {
        f =  rightRotate(n, i);
        struct args *factor = (struct args *)malloc(sizeof(struct args));
        factor->tid = i;
        factor->input = f;
        printf( "input: %d, tid: %d\n",factor->input,  factor->tid);
        pthread_create(&id[i],NULL,worker,(void *)factor);
        //printf("thread id created: %d, i:%d\n ", id[i],i);
   }
   
   // wait for all done
   int numFactors = 0;
   for (int i = 0; i < nthreads; i++)  {
       /*
       if (pthread_join(id[i],&work)){
           printf("ERROR");
       }
       */
      pthread_join(id[i],&work);
      printf("work: %d, id:%d, number passed:%d\n", work, id[i], i );
      numFactors += work;
   }
   printf("%d values of base done\n",numFactors);
   return 0;
}
