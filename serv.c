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
#define SLOT_SIZE 10

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
     

     // create memory and set metadata for client flag
     cfKEY     =    ftok("shmfile", 'x');
     cfID      =    shmget(cfKEY, sizeof(int), IPC_CREAT |0666);
     if (cfID < 0) {
          printf("*** shmget error (cf) ***\n");
          exit(1);
     }
     cfPTR     =    (int *) shmat(cfID, (void*)0, 0);
     if ((int) cfPTR == -1) {
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
     if ((int) numPTR == -1) {
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
     if ((int) sfPTR == -1) {
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
     if ((int) slotPTR == -1) {
          printf("*** shmat error (sf) ***\n");
          exit(1);
     }

     // initialise shared values
     int save;
     *cfPTR = 0;
     sfITER = sfPTR;
     for (int i = 0; i < SLOT_SIZE; i++){
          *sfITER = 0;
          printf("sserver client %d: %d %d\n", i, *sfITER, sfITER);
          sfITER++;
     }
     

     //set server flags to 0
     while (1){
          // if client has a request 
          if (*cfPTR == 1){
               // check if client quit (may not need)
               if (*numPTR == 0){
                   printf("this break\n");
                   *cfPTR = 0;
                   break;
               }
               // check if server has free slots via server flag
               sfITER = sfPTR;
               int freeSlot = -1;
               for (int i = 0; i < SLOT_SIZE; i++){
                    if (*sfITER == 0){
                         sfITER = 1;
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
                    *cfPTR = 0;
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
