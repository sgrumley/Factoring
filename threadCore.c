#include <stdio.h>
#include <math.h>
#include <pthread.h>  // required for threads usage

#define MAX_N 100000000
#define MAX_THREADS 320
#define INT_BITS 32
#define NTHREADS 32

// function to bit rotate right
int rightRotate(int n, unsigned int d){ 
   return (n >> d)|(n << (INT_BITS - d)); 
} 

// function to find factors
int trialDivision(int n){
    int f = 1;
    int ctr = 0;
    while(n >= f){
        if (n % f == 0){
            ctr++;
           // printf("factor: %d\n", f);
        } 
        f++;
    }
    return ctr;
}


// ID structs for the threads
pthread_t id[MAX_THREADS];
pthread_t tid;

// struct data for worker function
struct args {
    int tid;
    int input;
    int sid;
};

// struct for arguments 
struct arguments {
    int number;
    int slot;
};

// each thread runs this routine
void *factorisingInstance(void *data){   
    int fctr = 0;
    // send integer to be factorised - returns the amount of factors found
    fctr =  trialDivision(((struct args*)data)-> input);
    sleep(1);
    // free data
    free(data);
    return fctr;
}


void *factorization(void *data){
    int fctr;
    
    int n = ((struct arguments*)data)-> number;
    int sl= ((struct arguments*)data)-> slot;
    printf("factor for %d in slot %d\n", n, sl);

   // start 32 threads for each bit rotation on the number given
    int brr;
    for (int i = 0; i < NTHREADS; i++){
        brr =  rightRotate(n, i);
        struct args *factor = (struct args *)malloc(sizeof(struct args));
        factor->tid = i;
        factor->input = brr;
        factor->sid = sl;
        //printf( "input: %d, tid: %d\n",factor->input,  factor->tid);
        pthread_create(&id[i],NULL,factorisingInstance,(void *)factor);
   }
   
   // join all threads together and count total factors
   int numFactors = 0;
   for (int i = 0; i < NTHREADS; i++){
      pthread_join(id[i],&fctr);
      //printf("work: %d, id:%d, number passed:%d\n", fctr, id[i], i );
      numFactors += fctr;
   }
   printf("%d factors of %d done\n",numFactors, n);
   pthread_cancel(pthread_self());
}

int main(){  
    int te = 69;
    int slotid = 4;
    printf("Start\n");
    struct arguments *factorINIT = (struct arguments *)malloc(sizeof(struct arguments));
    factorINIT->number = te;
    factorINIT->slot = slotid;
    printf( "thread started for input: %d on slot: %d\n",factorINIT->number,  factorINIT->slot);
    pthread_create(&tid,NULL,factorization,(void *)factorINIT);
    while(1);
    
   return 0;
}
