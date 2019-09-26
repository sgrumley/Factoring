/* ----------------------------------------------------------------- */
/* PROGRAM  client.c                                                 */
/*    This is the client program.  It can only be started as the     */
/* server says so.  The client requests the same shared memory the   */
/* server established, attaches it to its own address space, takes   */
/* the data, changes the status to TAKEN, detaches the shared memory,*/
/* and exits.                                                        */
/* ----------------------------------------------------------------- */

#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

#include  "shm-02.h"

void  main(void)
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
     
     
     
     // define meta data around clientflag shared memory slot
     cfKEY = ftok(".", 'x');
     printf("%d\n", cfKEY);
     cfID = shmget(cfKEY, sizeof(int), 0666);
     if (cfID < 0) {
          printf("*** shmget error (cf) ***\n");
          exit(1);
     }     
     cfPTR = (int*) shmat(cfID, (void*)0, 0);
     if ((int) cfPTR == -1) {
          printf("*** shmat error (cf) ***\n");
          exit(1);
     }
     cf = cfPTR;
     printf("print cfstar%d\n",*cf);
     
     printf("cid%d\n", cfID);
     printf("cfptr %d\n", cfPTR);
     printf("   Cfptrstar: %d\n",*cfPTR);
     *cf = 11;
    int readIn;
    shmdt(cfPTR);
     
    exit(1);
     /*
     // define meta data around number share memory slot
     numKEY = ftok("shmfile1", 'c');
     numID = shmget(numKEY, sizeof(int), 0666);
     if (numID < 0) {
          printf("*** shmget error (num) ***\n");
          exit(1);
     }     
     numPTR = (int *) shmat(numID, NULL, 0);
     if ((int) numPTR == -1) {
          printf("*** shmat error (num) ***\n");
          exit(1);
     }
     // define meta data around server flags shared memory slot
     sfKEY = ftok("shmfile2", 'v');
     //printf("Tester %d", sizeof(sf));
     sfID = shmget(sfKEY, sizeof(int) *10, 0666);
     if (sfID < 0) {
          printf("*** shmget error (sf) ***\n");
          exit(1);
     }     
     sfPTR = (int *) shmat(sfID, NULL, 0);
     if ((int) sfPTR == -1) {
          printf("*** shmat error (sf) ***\n");
          exit(1);
     }
     

     //cfPTR = 0;
     
    
    while (1){
        if (cfPTR == 0){
             //check if there is an available slot
            //cfPTR  = 1;
            printf("Input a number: ");
            scanf("%d", &readIn);
            if (readIn == 0){
                //break;
            }
            numPTR = readIn;
            cfPTR = 1;          
         }

     }
    

    while (cfPTR != 0)
          sleep(1);

     shmdt((void *) cfPTR); // detach memory from program
     shmdt((void *) numPTR);
     shmdt((void *) sfPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(cfID, IPC_RMID, NULL); // destroy memory
     shmctl(numID, IPC_RMID, NULL); 
     shmctl(sfID, IPC_RMID, NULL); 
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
     */
}
