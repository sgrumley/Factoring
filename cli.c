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
     key_t          ShmKEY;
     int            ShmID;
     struct Memory  *ShmPTR;
     
     ShmKEY = ftok(".", 'x');
     ShmID = shmget(ShmKEY, sizeof(struct Memory), 0666);
     if (ShmID < 0) {
          printf("*** shmget error (client) ***\n");
          exit(1);
     }
     printf("   Client has received a shared memory of four integers...\n");
     
     ShmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
     if ((int) ShmPTR == -1) {
          printf("*** shmat error (client) ***\n");
          exit(1);
     }

     printf("   Client has attached the shared memory...\n");
    int readIn;
    while (1){
        if (ShmPTR->status  == TAKEN){
            ShmPTR->status  = NOT_READY;
            printf("Input a number: ");
            scanf("%d", &readIn);
            printf("here\n");
            if (readIn == 0){
                //break;
            }
            ShmPTR->data[0] = readIn;
            ShmPTR->status = FILLED;          
         }

     }
    ShmPTR->status  = NOT_READY;
    
    //printf("Client has filled %d %d %d %d to shared memory...\n",ShmPTR->data[0], ShmPTR->data[1], ShmPTR->data[2], ShmPTR->data[3]);
    

    while (ShmPTR->status != TAKEN)
          sleep(1);
          
     printf("Server has detected the completion of its child...\n");
     shmdt((void *) ShmPTR); // detach memory from program
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL); // destroy memory
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}
