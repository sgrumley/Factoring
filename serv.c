/* ----------------------------------------------------------------- */
/* PROGRAM  server.c                                                 */
/*    This program serves as the server similar to the one in shm.c. */
/* The difference is that the client is no more a child process.     */
/* Thus, some mechanism must be established between the server and   */
/* the client.  This program uses a naive one.  The shared memory    */
/* has an indicator whose possible values are NOT_READY, FILLED and  */
/* TAKEN.  Before the server completes filling data, the status is   */
/* NOT_READY, after filling data it is FILLED.  Then, the server just*/
/* wait (busy waiting) until the status becomes TAKEN.               */
/*    Note that the server must be started first.  The client can    */
/* only be started as the server says so.  Otherwise, the client may */
/* not be able to use the shared memory of the data therein.         */
/* ----------------------------------------------------------------- */

#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <math.h>
#include <pthread.h>

#include  "shm-02.h"

// ID structs for the threads
pthread_t id[MAX_THREADS];

struct args {
    int tid;
    int input;
    int slot;
};


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
            // move data to shared memory slot
            printf("f: %d\n", f);
        } 
        f++;
    }
    return ctr;
}

// each thread runs this routine
void *worker(void *data)  
{  int work = 0;
   int threadId = ((struct args*)data)-> tid;
   int threadInput = ((struct args*)data)-> input;
   //printf("id:%d, input:%d\n", threadId, threadInput);
   // push factors to slot memory
   work =  trialDivision(threadInput);
   sleep(1);
   free(data);
    return work;
   //return 1;
}

int factors(int n, int q){
    int work;
    //int n = 12345;
    int nthreads = 32;
    

   // get threads started
    int f;
    for (int i = 0; i < nthreads; i++)  {
        f =  rightRotate(n, i);
        struct args *factor = (struct args *)malloc(sizeof(struct args));
        factor->tid = i;
        factor->input = f;
        factor->slot = q;
        //printf( "input: %d, tid: %d\n",factor->input,  factor->tid);
        pthread_create(&id[i],NULL,worker,(void *)factor);
        //printf("thread id created: %d, i:%d\n ", id[i],i);
   }
   
   // wait for all done
   int numFactors = 0, progress = 0;
   int progressPercent;
   for (int i = 0; i < nthreads; i++)  {
       /*
       if (pthread_join(id[i],&work)){
           printf("ERROR");
       }
       */
      pthread_join(id[i],&work);
      //printf("work: %d, id:%d, number passed:%d\n", work, id[i], i );
      numFactors += work;
      progress++;
      // add progress variable to slotprogress shared memory
      progressPercent = (100 / nthreads) * progress;
      printf("Query -> %d -> %c%d\n", q,'%', progressPercent);
     // use as progress report i/32
   }
   // set slot value to finished
   // *slotPTR = QUERY_FIN

   printf("%d values of base done\n",numFactors);
   return 0;
}


void  main(){

     key_t     cfKEY;
     int       cfID;
     int       *cfPTR;

     key_t     sfKEY;
     int       sfID;
     int       sf[SLOT_SIZE];
     int       *sfPTR;
     int       *sfITER;

     key_t     numKEY;
     int       numID;
     int       *numPTR;

     key_t     slotKEY;
     int       slotID;
     int       slot[SLOT_SIZE];
     int       *slotPTR;
     int       *slotITER;
     

     // create memory and set metadata for client flag
     cfKEY     =    ftok("shmfile", 'x');
     cfID      =    shmget(cfKEY, sizeof(int), IPC_CREAT |0666);
     if (cfID < 0) {
          printf("*** shmget error (cf) ***\n");
          exit(1);
     }
     cfPTR     =    (int *) shmat(cfID, (void*)0, 0);
     if (*cfPTR == -1) {
          printf("*** shmat error (cf) ***\n");
          exit(1);
     }
     
     // create memory and set metadata for num
     numKEY    =    ftok("shmfile1", 'c');
     numID     =    shmget(numKEY, sizeof(int), IPC_CREAT | 0666);
     if (numID < 0) {
          printf("*** shmget error (num) ***\n");
          exit(1);
     }
     numPTR    =    (int *) shmat(numID, NULL, 0);
     if (*numPTR == -1) {
          printf("*** shmat error (num) ***\n");
          exit(1);
     }

     // create memory and set metadata for sf
     sfKEY     =    ftok("shmfile2", 'v');
     sfID      =    shmget(sfKEY, sizeof(int) * SLOT_SIZE, IPC_CREAT | 0666);
     if (sfID < 0) {
          printf("*** shmget error (sf) ***\n");
          exit(1);
     }
     // attaching to shared memory -> schmat (id, address, shmflg )
     sfPTR     =    (int *) shmat(sfID, NULL, 0);
     if (*sfPTR == -1) {
          printf("*** shmat error (sf) ***\n");
          exit(1);
     }

     // define meta data around server shared memory slot
     slotKEY   =    ftok(".", 'b');
     slotID = shmget(slotKEY, sizeof(int) * SLOT_SIZE, IPC_CREAT | 0666);
     if (slotID < 0) {
          printf("*** shmget error (sf) ***\n");
          exit(1);
     }     
     slotPTR   =    (int *) shmat(slotID, NULL, 0);
     if (*slotPTR == -1) {
          printf("*** shmat error (sf) ***\n");
          exit(1);
     }

     // initialise shared values
     int save;
     *cfPTR = 0;
     *numPTR = 0;
     sfITER = sfPTR;
     slotITER = slotPTR;
     for (int i = 0; i < SLOT_SIZE; i++){
          *sfITER = 0;
          *slotITER = 0;

          printf("sserver client %d: %d %d\n", i, *sfITER, sfITER);
          sfITER++;
          slotITER++;
     }
     

     //set server flags to 0
     while (1){
          // if client has a request 
          if (*cfPTR == FILLED){
               // check if client quit (may not need)
               if (*numPTR == 0){
                   printf("this break\n");
                   *cfPTR = WAITING;
                   break;
               }
               // check if server has free slots via server flag
               sfITER = sfPTR;
               int freeSlot = -1;
               for (int i = 0; i < SLOT_SIZE; i++){
                    if (*sfITER == WAITING){
                         *sfITER = FILLED;
                         freeSlot = i;
                         break;
                    }
                    sfITER++;
               }

               if (freeSlot != -1){
                    // read from num
                    save = *numPTR;
                    // call factoring function 
                    printf("getting factors of -%i-\n", save);
                    // replace num with index
                    *numPTR = freeSlot;
                    // set client flag back to ready to take
                    *cfPTR = TAKEN;
                    // save number to slot
                    *(slotPTR + freeSlot) = save;
                    //call factors
                    factors(save, freeSlot);
                    //print slots
                    sfITER = sfPTR;
                    slotITER = slotPTR;
                    printf("save %d to slot: %d\n", save, freeSlot);
                    for (int i = 0; i < SLOT_SIZE; i++){
                         printf("%d server flag : %d, Slot value: %d\n", i, *sfITER, *slotITER);
                         sfITER++;
                         slotITER++;
                    }
                    printf("\n------------------------------------\n");
               }           
             //*cfPTR = 0;
         }
     }
     printf("we founnd it %i",save);
     
      

     
     
     printf("Please start the client in another window...\n");
      //ShmPTR->status = TAKEN;
     printf("   Client has informed server data have been taken...\n");
     shmdt((void *) cfPTR);
     shmdt((void *) numPTR);
     shmdt((void *) sfPTR);
     printf("   Client has detached its shared memory...\n");
     printf("   Client exits...\n");
     exit(0);            
     
}
