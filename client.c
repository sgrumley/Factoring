/*waiter.c*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <errno.h>
int main()
{
const char *name   = "shared_memory";
const char *name2  = "shared_results";
const char *sema1  = "tSlot";
const char *sema2  = "aSlot";
const char *sema3  = "mutex";
const char *sema4  = "resMUT";
int shm_fd; //file descriptor of
int shm_res;
int *shelf;
int *res;
int loop=4;
sem_t *tSlot, *aSlot, *mutex;
sem_t *resMUT;


/* open the shared memory segment */
shm_fd = shm_open(name, O_RDWR, 0666);
shm_res = shm_open(name2, O_RDWR, 0666);

/* now map the shared memory segment in the address space of the
process */
shelf = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,shm_fd, 0);
res = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,shm_res, 0);
/*open semaphores*/
tSlot = sem_open(sema1, O_CREAT,0666,0);
aSlot = sem_open(sema2, O_CREAT, 0666, 2);
mutex = sem_open(sema3,O_CREAT,0666,1);
resMUT = sem_open(sema4,O_CREAT,0666,0);
int t= -1;
if (t == -1){


int readIn;
int chkSlots;
while(loop--){
     //chkSlots = sem_trywait(aSlot);
     //chkResult = 

     if (sem_trywait(aSlot) == 0){
          printf("Input a number: ");
          scanf("%d", &readIn);
          sem_wait(mutex);
          (*shelf) = readIn;          
          sem_post(mutex);
          sem_post(tSlot);
          sem_wait(resMUT);
          printf("recieved %d\n", (*res));
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
sem_unlink(sema1);
sem_unlink(sema2);
sem_unlink(sema3);
sem_unlink(sema4);
/*remove shared memory segment*/
munmap(shelf, sizeof(int));
munmap(res, sizeof(int));
close(shm_fd);
close(shm_res);
shm_unlink(name);
shm_unlink(name2);
return 0;
} 