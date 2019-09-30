#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#define NUM_SLOT 10
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
    sleep(2);
    printf("inti\n");
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

