#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include<string.h>
#include <fcntl.h>
void start_monitor()
{
   pid_t hub_mon=fork();

   if(hub_mon<0)
   {
       printf("Error creating child process\n");
       return;
   }

   else if (hub_mon==0)
   {
        printf("Child process created. PID = %d\n", getpid());
       //fork the monitor in the child process
        int fd[2];
        if(pipe(fd)<0)
        {
            //failure to pipe()
            exit(1);
        }
        //fork for monitor
        pid_t monitor_pid=fork();
        if(monitor_pid<0)
        {
            printf("Error creating child process->monitor_pid");
            return;
        }
        if(monitor_pid==0)
        {
            //close read file
            //we only need to keep the end of the pipe
            close(fd[0]);

            //redirect stdout to pipe
             dup2(fd[1], STDOUT_FILENO);

            //with execl we replace so we close it
            close(fd[1]);

            //replace the current procces with monitor_reports
            execl("./monitor_reports", "monitor_reports", NULL);

            //failure to execl
             perror("execl monitor_reports failure");
             exit(1);

        }
        if(monitor_pid>0)
        {
            close(fd[1]);
            char message[256];
            int size;
            while((size=read(fd[0],message,sizeof(message)-1))>0)
            {
                message[size]='\0';
                printf("%s",message);
                //we search for the error messages
                if(strstr(message,"Monitor already running")||strstr(message,"Monitor stopped"))
                {
                    printf("The monitor has stopped for a certain reason\n");
                    break;
                }


            }
            close(fd[0]);


            exit(0);
        }
   }
   else
   {
        printf("Parent process. Child PID = %d\n", hub_mon);

   }

}
//add id to add_report
//kill procces to make sure the process stops
//to add error case when we can't find a monitor
//add more errors

//kill the procces to make sure the hidden file .monitor_pid is deleted and it keeps the monitor_reports function the same
void stop_monitor()
{
      char string[120];
      snprintf(string,sizeof(string),".monitor_pid");
      int fd=open(".monitor_pid",O_RDONLY, 0644);
      if(fd==-1)
      {
        perror("eroare la deschidere fisier");
        return;
      }
      char buffer[120];
      int n=read(fd,buffer,sizeof(buffer)-1);
      buffer[n]='\0';
      int mon_pid=atoi(buffer);
      kill(mon_pid,SIGINT);
      close(fd);

}

int main(int argc,char *argv[])
{
    if(argc<2)
    {
         printf("Introduce a valid number of arguments!");
         exit(0);
    }
    if(strstr(argv[1],"start_monitor"))
    {
        start_monitor();
    }
    if(strstr(argv[1],"stop_monitor"))
    {
        stop_monitor();
    }
    return 0;
}
