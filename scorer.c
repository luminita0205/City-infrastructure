#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>
#define MAX 100
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

//for each score we need the name of the inspector and the severity level(the sum will be the nwe score)
typedef struct
{
    char name[30];
    int score;
}Score;
int main(int argc,char *argv[])
{
    if(argc !=2)
    {
        printf("Invalid arguments\n");
        exit(-1);
    }
    //reads each report from the current district
    char path[100];
    sprintf(path,"%s/reports.dat",argv[1]);

    FILE *f=fopen(path,"rb");
    if(f==NULL)
    {
        printf("Error opening file\n");
        exit(-1);

    }

    Score s[MAX];
    //number of inspectors
    int n=0;

    Report report;
    while(fread(&report,sizeof(Report),1,f)==1)
    {
        int index=-1;

        for(int i=0;i<n;i++)
        {
            if(strcmp(s[i].name,report.inspector_name)==0)
            {
                index=i;
                break;
            }
        }

        if(index==-1)
        {
             if(n >= MAX)
            {
                printf("Too many inspectors\n");
                break;
            }

            strcpy(s[n].name,report.inspector_name);
            s[n].score=report.severity_level;
            n++;
        }
        else
        {
            //if the inspector already exists the new score is score+=severity
            s[index].score+=report.severity_level;
        }
    }
    fclose(f);
    printf("District: %s\n", argv[1]);

    for(int i = 0; i < n; i++)
    {
        printf("%s %d\n", s[i].name, s[i].score);
    }
    return 0;
}
