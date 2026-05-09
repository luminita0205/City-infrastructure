#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include<stdlib.h>
#include <signal.h>
//pidul procesului care ruleaza programul
void createPidFile()
{
    int contorOk;
    ///wait for signals
    pid_t pid=getpid();
    char string[20];

     //create or overwrite
    contorOk=open(".monitor_pid", O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if(contorOk==-1)
    {
        perror("eroare la deschidere fisier");
        return;
    }

    sprintf(string,"%d",pid);
    write(contorOk,string,strlen(string));

     //!!important step
    close(contorOk);
}

//the program stops only when Ctrl+C is pressed -> SIGINT
void handler(int semnal)
{
    //signal used to stop the program
    if(semnal==SIGINT)
    {
    printf("Monitor stopped\n");
    unlink(".monitor_pid");
    exit(0);
    }
    //signal received when a new report is added
    else if(semnal==SIGUSR1)
    {
          printf("New report added!\n");
    }
}



int main(void)
{
     createPidFile();
     //the operating system automatically sends the signal number to the handler
    //sigaction structure used for signal handling
     struct sigaction act;
     act.sa_flags = 0;
     act.sa_handler = &handler;


     sigaction(SIGINT,&act,NULL);
     sigaction(SIGUSR1,&act,NULL);
     while(1)
     {
         pause();
         //wait for signals
     }
     return 0;
}
