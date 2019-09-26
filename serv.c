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

#include  "shm-02.h"

void  main(int  argc, char *argv[])
{
     key_t          cfKEY;
     int            cfID;
     int            *cfPTR;
     int *cf;

     //key_t          sfKEY;
     //int            sfID;
     //int            sf[10];
     //int            *sfPTR;
     //key_t          numKEY;
     //int            numID;
     //int            *numPTR;
     

     // create memory and set metadata for client flag
     cfKEY = ftok(".", 'x');
     printf("%d\n", cfKEY);
     cfID = shmget(cfKEY, sizeof(int), IPC_CREAT |0666);
     if (cfID < 0) {
          printf("*** shmget error (cf) ***\n");
          exit(1);
     }
     // attaching to shared memory -> schmat (id, address, shmflg )
     cfPTR = (int *) shmat(cfID, (void*)0, 0);
     if ((int) cfPTR == -1) {
          printf("*** shmat error (cf) ***\n");
          exit(1);
     }
     cf = cfPTR;
     *cf = 2;
     printf("%d\n",cfPTR);
     //cfPTR = 12;
     printf("%d\n", cfID);
     printf("%d\n",cfPTR);
    
     sleep(10);
     printf("cfptr %d\n",cfPTR);
     printf("cfptr* %d\n",*cfPTR);
     shmdt(cfPTR);
     shmctl(cfID,IPC_RMID,NULL); 
    
     exit(1);
     int save;
/*
     // create memory and set metadata for num
     numKEY = ftok("shmfile1", 'c');
     numID = shmget(numKEY, sizeof(int), IPC_CREAT | 0666);
     if (numID < 0) {
          printf("*** shmget error (num) ***\n");
          exit(1);
     }
     
     numPTR = (int *) shmat(numID, NULL, 0);
     if ((int) numPTR == -1) {
          printf("*** shmat error (num) ***\n");
          exit(1);
     }

     // create memory and set metadata for sf
     sfKEY = ftok("shmfile2", 'v');
     sfID = shmget(sfKEY, sizeof(int)*10, IPC_CREAT | 0666);
     if (sfID < 0) {
          printf("*** shmget error (sf) ***\n");
          exit(1);
     }
     // attaching to shared memory -> schmat (id, address, shmflg )
     sfPTR = (int *) shmat(sfID, NULL, 0);
     if ((int) sfPTR == -1) {
          printf("*** shmat error (sf) ***\n");
          exit(1);
     }
     
     
     
     while (1){
          
         if (cfPTR == 1){
              //printf("%d\n",cfPTR);
              printf("found data\n");
              save = numPTR;
              if (save == 0){
                   printf("this break\n");
                   cfPTR = 0;
                   break;
              }
             printf("getting factors of -%i-\n", save);
             cfPTR = 0;
             
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
     */
}
