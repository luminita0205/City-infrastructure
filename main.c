
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#define SIZE 150

typedef struct
{
    int report_id;
    char inspector_name[30];
    float latitude;
    float longitude;
    char issue_category[30];
    int severity_level;
    time_t timp;
    char description[100];
} Report;
//verifica inainte sa dechizi fisere cu set daca are permisiunea buna
//stat()->aici am ramas
int verifica_permisiuni(char *fisier, int perm)
{
    struct stat st;

    if(stat(fisier,&st) == -1)
    {
        printf("Eroare stat\n");
        return 0;
    }

    if((st.st_mode & 0777) == perm)
    {
        return 1;
    }

    return 0;
}

FILE *deschide_fisier(char *nume, char *mod)
{
    FILE *f = fopen(nume, mod);

    if(f == NULL)
    {
        perror(nume);
        exit(1);
    }

    return f;
}

int main(int argc,char *argv[])
{
    if(argc!=7)
    {
      fprintf(stderr,"Eroare la argumente");
      exit(-1);
    }
    else
    {
        printf("Good job\n");
    }

    if(strcmp(argv[1],"--role")==0)
    {
        if(strcmp(argv[2],"inspector")==0)
        {
            printf("are rolul de %s\n",argv[2]);
        }
        if(strcmp(argv[2],"manager")==0)
        {
            printf("are rolul de %s\n",argv[2]);
        }

    }

    if(strcmp(argv[3],"--user")==0)
    {
        printf("userul are numele %s\n",argv[4]);
    }
     //creem director separat pentru fiecare district
     //foar cu functii c
      char *dirname =malloc(SIZE*sizeof(char));
      if(dirname==NULL)
      {
          fprintf(stderr,"Eroare la alocare\n");
          exit(-1);
      }
      
     strcpy(dirname,argv[6]);

      if(mkdir(dirname)==0)
      {
          printf("S-a creat cu succes!\n");
      }
      else
      {
             printf("Directorul exista deja.\n");
      }
    chmod(dirname,0750);
    char path1[200];
    char path2[200];
    char path3[200];
    
    sprintf(path1,"%s/reports.dat",dirname);
    sprintf(path2,"%s/district.cfg",dirname);
    sprintf(path3,"%s/logged_district",dirname);
    
    FILE *f  = deschide_fisier(path1,"ab");
    FILE *f1 = deschide_fisier(path2,"a");
    //a->append
    FILE *f2 = deschide_fisier(path3,"a");

    chmod(path1,0664);
    chmod(path2,0640);
    chmod(path3,0644);

   
    //scriere de xemple in fisier->intreaba daca trebe
    Report report;
    report.report_id = 1;
    strcpy(report.inspector_name, argv[4]);
    report.latitude = 45.75;
    report.longitude = 21.22;
    strcpy(report.issue_category,"road");
    report.severity_level = 3;
    report.timp = time(NULL);
    strcpy(report.description,"Groapa mare");
    fwrite(&report,sizeof(Report),1,f);

   
    fprintf(f1,"threshold=2\n");

    fclose(f1);
    f1 = fopen(path2,"r");
    int threshold;
    fscanf(f1,"threshold=%d",&threshold);
        

    if(report.severity_level >= threshold)
    {
        printf("ALERTA!\n");
    }
     else
    {
        printf("Nu este alerta\n");
    }

    time_t now = time(NULL);

    fprintf(f2,"[%ld] %s %s %s %s\n",
    now,argv[2], argv[4],argv[5],  argv[6]);  

    //exemplu sa vad ca merge
    fprintf(f2,"[time] inspector bob add downtown\n");
    fclose(f2);
    f2 = fopen(path3,"r");

    char linie[200];

    while(fgets(linie, sizeof(linie), f2) != NULL)
    {
        printf("%s", linie);
    }

    fclose(f);
    fclose(f1);
    fclose(f2);
    free(dirname);
    return 0;
}
