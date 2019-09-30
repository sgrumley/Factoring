/*waiter.c*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
#define NUM_SLOT 10
int main()
{
const char *name   = "shared_memory";
const char *name2  = "shared_results";
const char *name3   = "shared_slot";
const char *sema1  = "tSlot";
const char *sema2  = "aSlot";
const char *sema3  = "mutex";
const char *sema4  = "resMUT";
const char *sema5   = "slotReady";
const char *sema6   = "slotWaiting";
const char *sema7   = "slotMUT";
int shm_fd; //file descriptor of
int shm_res;
int shm_slot;
int *shelf;
int *res;
int *slot;
int loop=4;
sem_t *tSlot, *aSlot, *mutex;
sem_t *resMUT;
sem_t *slotReady, *slotWaiting, *slotMUT;

/* open the shared memory segment */
shm_fd    = shm_open(name, O_RDWR, 0666);
shm_res   = shm_open(name2, O_RDWR, 0666);
shm_slot  = shm_open(name3, O_RDWR, 0666);

/* now map the shared memory segment in the address space of the
process */
shelf     = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,shm_fd, 0);
res       = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,shm_res, 0);
slot      = mmap(0,sizeof(int) * NUM_SLOT, PROT_READ | PROT_WRITE, MAP_SHARED,shm_slot, 0);

/*open semaphores*/
tSlot          = sem_open(sema1, O_CREAT,0666,0);
aSlot          = sem_open(sema2, O_CREAT,0666,3);
mutex          = sem_open(sema3, O_CREAT,0666,1);
resMUT         = sem_open(sema4, O_CREAT,0666,0);
slotMUT        = sem_open(sema5, O_CREAT,0666,1);
slotReady      = sem_open(sema6, O_CREAT,0666,1);
slotWaiting    = sem_open(sema7, O_CREAT,0666,0);
int t= -1;
if (t == -1){

int queries[100];
for (int i=0; i<100; i++){
     queries[i]=-1;
}

int readIn;

int chkSlots;
while(loop--){
     if (sem_trywait(aSlot) == 0){
          printf("Input a number: ");
          scanf("%d", &readIn);
          sem_wait(mutex);
          (*shelf) = readIn;          
          sem_post(mutex);
          sem_post(tSlot);
          

          sem_wait(resMUT);
          int qid;
          //search for query id
          for (int a=0; a<100; a++){
               if (queries[a] == -1){
                    qid = a;
                    break;
               }
          }
          // search for query id
          printf("slot %d for query %d\n", (*res), qid);
          queries[qid] = (*res);
          sem_post(resMUT);
     } else {
          printf("server is busy\n");
          sem_wait(aSlot);
     }
     
}
} // test
/*remove semaphores*/
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
/*remove shared memory segment*/
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