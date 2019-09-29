#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
int main()
{
    
const char *name    = "shared_memory";
const char *name2   = "shared_results";
const char *sema1   = "tSlot";
const char *sema2   = "aSlot";
const char *sema3   = "mutex";
const char *sema4   = "resMUT";
int shm_fd; //shared memory file discriptor
int shm_res;
int *shelf;
int *res;
int loop=4;
sem_t *tSlot, *aSlot, *mutex;
sem_t *resMUT;
/* make *shelf shared between processes */
//create the shared memory segment

shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
shm_res = shm_open(name2, O_CREAT | O_RDWR, 0666);
//configure the size of the shared memory segment
ftruncate(shm_fd,sizeof(int));
ftruncate(shm_res,sizeof(int));

//map the shared memory segment in process address space
shelf   = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
res     = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_res, 0);
/*creat/open semaphores*/
//cook post semaphore tSlot after cooking a pizza
tSlot = sem_open(sema1, O_CREAT,0666,0);
aSlot = sem_open(sema2, O_CREAT, 0666, 2); 
//mutex for mutual exclusion of shelf
mutex = sem_open(sema3,O_CREAT,0666,1);
resMUT = sem_open(sema4, O_CREAT, 0666, 1); 
int t = -1;
if (t == -1){
int ctr=5;

while(loop--){
    sem_wait(tSlot);    
    sem_wait(mutex);
    printf("factorise: %d\n", *shelf);
    sleep(2);
    sem_post(mutex);
    sem_post(aSlot);
    sem_wait(resMUT);
    printf("inti\n");
    (*res) = ctr;
    sem_post(resMUT);
    ctr++;
    


}
} //test
/*close and unlink semaphores*/
sem_close(tSlot);
sem_close(aSlot);
sem_close(mutex);
sem_close(resMUT);
sem_unlink(sema1);
sem_unlink(sema2);
sem_unlink(sema3);
sem_unlink(sema4);
/*close and unlink shared memory*/
munmap(shelf, sizeof(int));
munmap(res, sizeof(int));
close(shm_fd);
close(shm_res);
shm_unlink(name);
shm_unlink(name2);
return 0;
} 

