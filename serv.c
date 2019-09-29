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
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <errno.h>

#include <unistd.h>

#include <sys/mman.h>

#include  "shm-02.h"

// ID structs for the threads
pthread_t id[MAX_THREADS];
pthread_t tid;

// struct data for worker function
struct args {
    int tid;
    int input;
    int sid;
    int *slotPTR;
    sem_t *slotSEM;
    int *flagPTR;
};

// struct for arguments 
struct arguments {
    int number;
    int slot;
    int *slotPTR;
    sem_t *slotSEM;
    int *flagPTR;
};


int rightRotate(int n, unsigned int d){ 
   /* In n>>d, first d bits are 0. To put last 3 bits of at  
     first, do bitwise or of n>>d with n <<(INT_BITS - d) */
   return (n >> d)|(n << (INT_BITS - d)); 
} 

// each thread runs this routine
void *factorisingInstance(void *data){   
    int n = ((struct args*)data)-> input;
    sem_t *sem = ((struct args*)data)-> slotSEM;
    int *slotADDR = ((struct args*)data)-> slotPTR;
    int *fp = ((struct args*)data)-> flagPTR;
    // send integer to be factorised - returns the amount of factors found
    int f = 1;
    int ctr = 0;
    while(n >= f){
        if (n % f == 0){
            ctr++;
            sem_wait(sem);
               *slotADDR = f;
               *fp = FILLED;
            sem_post(sem);
            // move data to shared memory slot
            //printf("f: %d\n", f);
        } 
        f++;
    }
    // free data
    free(data);
    return ctr;
}

void *factorization(void *data){
    int fctr;
    //printf("make it");
    
    int n = ((struct arguments*)data)-> number;
    int sl= ((struct arguments*)data)-> slot;
    printf("factor for %d in slot %d\n", n, sl);
printf("%d - %d \n",*(((struct arguments*)data)-> slotPTR), *(((struct arguments*)data)-> slotSEM) );
printf("broken %d %d\n", ((struct arguments*)data)-> flagPTR), *(((struct arguments*)data)-> flagPTR);
    printf("%p - %p - %p\n",((struct arguments*)data)-> slotPTR, ((struct arguments*)data)-> slotSEM,((struct arguments*)data)-> flagPTR );

   // start 32 threads for each bit rotation on the number given
    int brr;
    for (int i = 0; i < NTHREADS; i++){
        brr =  rightRotate(n, i);
        struct args *factor = (struct args *)malloc(sizeof(struct args));
        factor->tid = i;
        factor->input = brr;
        factor->sid = ((struct arguments*)data)-> slot;
        factor->slotPTR = ((struct arguments*)data)-> slotPTR;
        factor->slotSEM = ((struct arguments*)data)-> slotSEM;
        factor->flagPTR = ((struct arguments*)data)-> flagPTR;
        pthread_create(&id[i],NULL,factorisingInstance,(void *)factor);
   }   
   // join all threads together and count total factors
   int numFactors = 0, progress = 0;
   int progressPercent;
   for (int i = 0; i < NTHREADS; i++){
      pthread_join(id[i],&fctr);
      //printf("work: %d, id:%d, number passed:%d\n", fctr, id[i], i );
      progress++;
      // add progress variable to slotprogress shared memory
      progressPercent = (100 / NTHREADS) * progress;
      numFactors += fctr;
      //printf("Query -> %d -> %c%d\n", sl,'%', progressPercent);
   }
   printf("%d factors of %d done for slot %d\n",numFactors, n, sl);
   *((struct arguments*)data)-> flagPTR = QUERY_FIN;
   pthread_cancel(pthread_self());
}




void  main(){

     key_t     cfKEY;
     sem_t     *cfSEM;
     int       cfID;
     int       *cfPTR;
     //const char *sema1 = "clientSEM";


     key_t     sfKEY;
     sem_t     *sfSEM;
     int       sfID;
     int       sf[SLOT_SIZE];
     int       *sfPTR;
     int       *sfITER;

     key_t     numKEY;
     sem_t     *numSEM;
     int       numID;
     int       *numPTR;

     key_t     slotKEY;
     sem_t     *slotSEM;
     int       slotID;
     int       slot[SLOT_SIZE];
     int       *slotPTR;
     int       *slotITER;

     int       value;
     /*
     const char *sema1= "fill";
     const char *sema2= "avail";
     const char *sema3="mutex";
     */
    sem_unlink("numSEM");
    sem_unlink("sfSEM");
    sem_unlink("cfSEM");
    sem_unlink("slotSEM");

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
     cfSEM = sem_open("cfSEM", O_CREAT, 0666, 1);
     sem_getvalue(cfSEM, &value);
     printf("cf init:%d\n", value);
     
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
     numSEM = sem_open("numSEM", O_CREAT, 0666, 1);
     sem_getvalue(numSEM, &value);
     printf("num init:%d\n", value);

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
     sfSEM = sem_open ("sfSEM", O_CREAT, 0666, 1);
     sem_getvalue(sfSEM, &value);
     printf("sf init:%d\n", value);

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
     slotSEM = sem_open ("slotSEM", O_CREAT, 0666, 1);
     sem_getvalue(slotSEM, &value);
     printf("slot init:%d\n", value);

     // initialise shared values
     int input;
     sem_wait(cfSEM);
     *cfPTR = 0;
     sem_post(cfSEM);
     //*numPTR = 0;
     sfITER = sfPTR;
     slotITER = slotPTR;
     sem_wait(slotSEM);
     for (int i = 0; i < SLOT_SIZE; i++){
          *sfITER = 0;
          *slotITER = 0;

          printf("sserver client %d: %d %d\n", i, *sfITER, sfITER);
          sfITER++;
          slotITER++;
     }
     sem_post(slotSEM);

     //set server flags to 0
     while (1){
          // if client has a request 
          if (*cfPTR == FILLED){
               // check if client quit (may not need)
               if (*numPTR == 0){
                   sem_wait(cfSEM);
                   *cfPTR = WAITING;
                   sem_post(cfSEM);
                   break;
               }
               // check if server has free slots via server flag
               sfITER = sfPTR;
               int freeSlot = -1;
               for (int i = 0; i < SLOT_SIZE; i++){
                    if (*sfITER == WAITING){
                         sem_wait(sfSEM);
                         // potensially sfPTR + i = FILLED
                         *sfITER = FILLED;
                         sem_post(sfSEM);
                         freeSlot = i;
                         break;
                    }
                    sfITER++;
               }

               if (freeSlot != -1){
                    // read from num
                    sem_wait(cfSEM);
                    input = *numPTR;                    
                    // replace num with index
                    *numPTR = freeSlot;
                    printf("getting factors of -%i-\n", input);
                    // set client flag back to ready to take
                    *cfPTR = TAKEN;
                    // save number to slot
                    *(slotPTR + freeSlot) = input;
                    //call factors
                     sem_post(cfSEM);
                     int *he = sfPTR + freeSlot;
                    struct arguments *factorINIT = (struct arguments *)malloc(sizeof(struct arguments));
                    factorINIT->number = input;
                    factorINIT->slot = freeSlot;
                    factorINIT->slotPTR = slotPTR + freeSlot;
                    factorINIT->slotSEM = slotSEM;
                    factorINIT->slotPTR = he;
                    //printf("herherh %p", sfPTR + freeSlot);
                    printf( "thread started for input: %d on slot: %d\n",factorINIT->number,  factorINIT->slot);
                    pthread_create(&tid,NULL,factorization,(void *)factorINIT);
                    //print slots
                    sem_wait(sfSEM);
                    sfITER = sfPTR;
                    slotITER = slotPTR;
                    printf("save %d to slot: %d\n", input, freeSlot);
                    for (int i = 0; i < SLOT_SIZE; i++){
                         printf("%d server flag : %d, Slot value: %d\n", i, *sfITER, *slotITER);
                         sfITER++;
                         slotITER++;
                    }
                    sem_post(sfSEM);
                    printf("\n------------------------------------\n");
               }           
         }
     }
     
     // end session
     shmdt((void *) cfPTR);
     shmdt((void *) numPTR);
     shmdt((void *) sfPTR);
     printf("   Client has detached its shared memory...\n");
     printf("   Client exits...\n");
     exit(0);            
     
}
