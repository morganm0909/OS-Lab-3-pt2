#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define SHMKEY ((key_t) 1497)

typedef struct {
  int bankAcct;
  int turn;
}memory;


void deposit(int balance, memory *account){
  if (balance % 2 == 0){
    account -> bankAcct += balance;
    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account -> bankAcct);
  }
  else{
    printf("Dear old Dad: Doesn't have any money to give\n");
  }
}

void withdraw(int balance, memory *account){
  if (balance <= account -> bankAcct){
    account -> bankAcct += balance;
    printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account -> bankAcct);
  }
  else{
    printf("Poor Student: Not Enough Cash ($%d)\n", account -> bankAcct );
  }
}

int  main(int  argc, char *argv[]){
     int    ShmID;
     memory    *ShmPTR;
     int pid;


    ShmID = shmget(SHMKEY, sizeof(memory), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }

     ShmPTR = (memory *)shmat(ShmID,NULL,0);
     ShmPTR -> bankAcct = 0;
     ShmPTR -> turn = 0;

     pid = fork();
     if (pid == 0) {
        int i;
        for(i=0;i<25;i++){
          sleep(rand()%6);
          while(ShmPTR -> turn != 1){}
          int balance = rand()%51;
          printf("Poor Student needs $%d\n", balance);
          withdraw(balance, ShmPTR);
          ShmPTR -> turn = 0;
        }
     }
     else{
        int i;
        for(i=0;i<25;i++){
          sleep(rand()%6);
          while(ShmPTR -> turn != 0){}
          int balance = rand()%101;
          if (ShmPTR -> bankAcct <= 100){
            deposit(balance,ShmPTR);
          }
          else{
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", balance);
          }
          ShmPTR -> turn = 1;
        }




        wait(NULL);
        shmdt(ShmPTR);
        shmctl(ShmID, IPC_RMID, NULL);
     }
     return 0;
     

}
