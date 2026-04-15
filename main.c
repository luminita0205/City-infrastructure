
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 150


int main(int argc,char *argv[])
{
    if(argc!=5)
    {
      fprintf(stderr,"Eroare la argumente");
      exit(-1);
    }
    else
    {
        printf("Good job");
    }

    if(strcmp(argv[1],"role")==0)
    {
        if(strcmp(argv[2],"inspector")==0)
        {
            printf("are rolul de %s",argv[2]);
        }
        if(strcmp(argv[2],"manager")==0)
        {
            printf("are rolul de %s",argv[2]);
        }

    }
     //creem director separat pentru fiecare district
     //foar cu functii c
      char *dirname =malloc(SIZE*sizeof(char));
      if(dirname==NULL)
      {
          fprintf(stderr,"Eroare la alocare\n");
          exit(-1);
      }
      strcpy(dirname,"/home/student/ProiectSO/");
      strcat(dirname,argv[4]);

      if(mkdir(dirname,0777)==0)
      {
          printf("S-a creat cu succes!\n");
      }
      else
      {
          fprintf(stderr,"Nu s-a creat:(\n");
          exit(-1);
      }
    free(dirname);
    return 0;
}
