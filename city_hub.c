#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include<string.h>

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
int main(void)
{
    start_monitor();
    return 0;
}
