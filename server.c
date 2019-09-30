#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <pthread.h>

#define     NUM_SLOT 10
#define 	NTHREADS 32

// ID for threads
pthread_t id[NTHREADS];
pthread_t tid;

// struct for args 
struct args {
    int number;
    int *slotPTR;
    int *flagPTR;
    sem_t *slotSEM;
};

// right bit rotation
int rightRotate(int n, unsigned int d){ 
   return (n >> d)|(n << (NTHREADS - d)); 
} 

// handles each factorisation computation
void *factorisingInstance(void *data){   
    int f = 1;
    int ctr = 0;    
    // localise variables
    int n = ((struct args*)data)-> number;
    sem_t *sem = ((struct args*)data)-> slotSEM;
    int *slotADDR = ((struct args*)data)-> slotPTR;
    //int *fp = ((struct args*)data)-> flagPTR;
    
    while(n >= f){
        if (n % f == 0){
            ctr++;
            //sem_wait(sem);
            //*slotADDR = f;
            //fp = FILLED;
            //sem_post(sem);
            // move data to shared memory slot
            //printf("f: %d\n", f);
        } 
        f++;
    }
    // free data
    free(data);
    return ctr;
}


// handles each server request on a new thread
void *factorization(void *data){
    int fctr;
    // localise variables
    int n = ((struct args*)data)-> number;
    // start 32 threads for each bit rotation on the number given
    int brr;
    // create
    for (int i = 0; i < NTHREADS; i++){
        brr =  rightRotate(n, i);
        struct args *factor = (struct args *)malloc(sizeof(struct args));
        factor->number  = brr;
        factor->slotPTR = ((struct args*)data)-> slotPTR;
        factor->slotSEM = ((struct args*)data)-> slotSEM;
        //factor->flagPTR = ((struct args*)data)-> flagPTR;
        pthread_create(&id[i],NULL,factorisingInstance,(void *)factor);
   }
   int numFactors = 0, progress = 0;
   int progressPercent;
   for (int i = 0; i < NTHREADS; i++){
        // join all threads together and count total factors
        pthread_join(id[i],&fctr);
        numFactors += fctr;
        progress++;
        // add progress variable to slotprogress shared memory
        progressPercent = (100 / NTHREADS) * progress;
        // write to shared memory slots progression[10]
        // client can query timed
        printf("Query -> %c%d\n",'%', progressPercent);
   }

   free(data);
   printf("%d total factors of %d\n",numFactors, n);
   *((struct args*)data)-> slotPTR = 0;
   pthread_cancel(pthread_self());
}



int main()
{
    
const char *name    = "shared_memory";
const char *name2   = "shared_results";
const char *name3   = "shared_slot";
const char *sema1   = "tSlot";
const char *sema2   = "aSlot";
const char *sema3   = "mutex";
const char *sema4   = "resMUT";
const char *sema5   = "slotReady";
const char *sema6   = "slotWaiting";
const char *sema7   = "slotMUT";
int shm_fd; //shared memory file discriptor
int shm_res;
int shm_slot;
int *shelf;
int *res;
int *slot;
int loop=4;
sem_t *tSlot, *aSlot, *mutex;
sem_t *resMUT;
sem_t *slotReady, *slotWaiting, *slotMUT;
/* make *shelf shared between processes */
//create the shared memory segment

shm_fd      = shm_open(name, O_CREAT | O_RDWR, 0666);
shm_res     = shm_open(name2, O_CREAT | O_RDWR, 0666);
shm_slot    = shm_open(name3, O_CREAT | O_RDWR, 0666);

//configure the size of the shared memory segment
ftruncate(shm_fd,sizeof(int));
ftruncate(shm_res,sizeof(int));
ftruncate(shm_slot,sizeof(int) * NUM_SLOT);

//map the shared memory segment in process address space
shelf   = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
res     = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_res, 0);
slot    = mmap(0,sizeof(int) * NUM_SLOT, PROT_READ | PROT_WRITE, MAP_SHARED, shm_slot, 0);

/*creat/open semaphores*/
//cook post semaphore tSlot after cooking a pizza
tSlot       = sem_open(sema1, O_CREAT,0666,0);
aSlot       = sem_open(sema2, O_CREAT, 0666, 3);
mutex       = sem_open(sema3,O_CREAT,0666,1);
resMUT      = sem_open(sema4, O_CREAT, 0666, 1);  
slotMUT     = sem_open(sema5,O_CREAT,0666,1);
slotReady   = sem_open(sema6,O_CREAT,0666,1);
slotWaiting = sem_open(sema7,O_CREAT,0666,0);
//mutex for mutual exclusion of shelf

int t = -1; //test
if (t == -1){
// initialise slots
for (int i=0; i<NUM_SLOT; i++){
    *(slot + i) = 0;
}
*(slot) = 1;
int currentSlot;
while(loop--){
    sem_wait(tSlot);  
    // find slot
    for (int i=0; i<NUM_SLOT; i++){
        if (*(slot + i) == 0){
            currentSlot = i;
            *(slot + i) = 1;
            break;
        } else {
            currentSlot = -1;
        }
    }

    // find slot  
    sem_wait(mutex);
    printf("factorise: %d\n", *shelf);
    // call factorise function
    // requires a pointer to the specific slot, number to factorise, and a pointer to progression slot
    struct args *factor = (struct args *)malloc(sizeof(struct args));
    factor->number  = *(shelf); 
    factor->slotPTR = (slot + currentSlot);
    factor->slotSEM = slotMUT;
    //factor->flagPTR = flagPTR;
    pthread_create(&tid,NULL,factorization,(void *)factor);
    sem_wait(resMUT);
    (*res) = currentSlot;
    sem_post(resMUT); 
    sem_post(mutex);    
    sem_post(aSlot);
     


}
} //test
/*close and unlink semaphores*/
sem_close(tSlot);
sem_close(aSlot);
sem_close(mutex);
sem_close(resMUT);
sem_close(slotMUT);
sem_close(slotWaiting);
sem_close(slotReady);
sem_unlink(sema1);
sem_unlink(sema2);
sem_unlink(sema3);
sem_unlink(sema4);
sem_unlink(sema5);
sem_unlink(sema6);
sem_unlink(sema7);
/*close and unlink shared memory*/
munmap(shelf, sizeof(int));
munmap(res, sizeof(int));
munmap(slot, sizeof(int)*10);
close(shm_slot);
close(shm_fd);
close(shm_res);
shm_unlink(name);
shm_unlink(name2);
shm_unlink(name3);
return 0;
} 

