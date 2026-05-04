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
    //se ia pidul de la fisier
    pid_t pid=getpid();
    char string[20];

    //il creeaza sau il suprascrie
    contorOk=open(".monitor_pid", O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if(contorOk==-1)
    {
        perror("eroare la deschidere fisier");
        return;
    }

    sprintf(string,"%d",pid);
    write(contorOk,string,strlen(string));

    //eliberez descriptor important!!
    close(contorOk);
}

//sterge doar la ctrl+c
void handle_Sigint(int semnal)
{
    printf("Monitor stopped\n");
    unlink(".monitor_pid");
    exit(0);
}

//semnal pentru add new report
void handle_Sigusr1(int semnal)
{
    printf("New report added!\n");
}

int main(void)
{
     createPidFile();
     //functia primeste automat param de la sistemul de operare
     signal(SIGINT,handle_Sigint);
     signal(SIGUSR1, handle_Sigusr1);
     while(1)
     {
         pause();
         //asteapta semnale
     }
     return 0;
}
