#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//da constante pentru permisiunea intreab
//S_IRUSR -> macrou pentro cod de permisiune
//man 2 chmod
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
int checkWithStat(char *fisier, int perm)
{
    struct stat st;

    if(stat(fisier,&st) == -1)
    {
        printf("Bad Stat Info\n");
        return 0;
    }

    if(st.st_mode & perm)
    {
        return 1;
    }

    return 0;
}

FILE *openFile(char *nume, char *mode)
{
    FILE *f = fopen(nume, mode);

    if(f == NULL)
    {
        perror(nume);
        exit(1);
    }

    return f;
}

//creare permisiune cu 0000
void createEmptyFile(char *nume)
{
    int fd = open(nume, O_CREAT | O_RDWR, 0000);

    if(fd == -1)
    {
        perror(nume);
        exit(1);
    }

    close(fd);
}

//verificare de permisiuni ->man 2 chmod nu uita
//intreab daca asta trebe la list sau e ok sa precizez de aici
void checkPermissions(char *role, char *path1, char *path2, char *path3)
{
    //reports.dat
    if (strcmp(role, "inspector") == 0)
    {
        printf("Inspector can read and write reports.dat\n");

        if (!checkWithStat(path1, S_IWUSR))
        {
            printf("Access denied:Inspector cannot write in reports.dat\n");
            exit(1);
        }
    }
    else if (strcmp(role, "manager") == 0)
    {
        printf("Manager can read and write reports.dat\n");
    }

    //district.cfg
    if (strcmp(role, "manager") == 0)
    {
        printf("Manager can read and write district.cfg\n");
    }
    else if (strcmp(role, "inspector") == 0)
    {
        printf("Inspector can only read district.cfg\n");
    }

    if (!checkWithStat(path2, S_IRUSR))
    {
        printf("No read permission for district.cfg\n");
        exit(1);
    }

    //logged_district
    if (strcmp(role, "manager") == 0)
    {
        printf("Manager can read and write logged_district\n");
    }
    else if (strcmp(role, "inspector") == 0)
    {
        printf("Inspector can only read logged_district\n");
    }
}
//convert perm to string
void permissionToString(mode_t mode, char string[])
{
    //R-read USR-user
    if(mode & S_IRUSR) string[0]='r';
    else string[0]='-';

    if(mode & S_IWUSR) string[1]='w';
    else string[1]='-';

    if(mode & S_IXUSR) string[2]='x';
    else string[2]='-';

    if(mode & S_IRGRP) string[3]='r';
    else string[3]='-';
    //W-group
    if(mode & S_IWGRP) string[4]='w';
    else string[4]='-';

    if(mode & S_IXGRP) string[5]='x';
    else string[5]='-';

    if(mode & S_IROTH) string[6]='r';
    else string[6]='-';

    if(mode & S_IWOTH) string[7]='w';
    else string[7]='-';

    if(mode & S_IXOTH) string[8]='x';
    else string[8]='-';

    string[9]='\0';
}
//list
//./prog --role inspector --user ana --list downtown
void listCommand(char *fisier)
{
    struct stat st;
    //permisiunea
    char string[10];

    if(stat(fisier,&st)==-1)
    {
        printf("Stat error\n");
        return;
    }

    permissionToString(st.st_mode,string);
    printf("\n");
    printf("LISTCOMMAND:\n");
    printf("Permissions: %s\n",string);
    printf("Size: %ld bytes\n",st.st_size);
    printf("Last modified: %s",ctime(&st.st_mtime));
}

//in add verrifc daca exista director
//cele 3 fisiere
//permisiuni verificare
//link simbolic
void addReport(char *fisier)
{
    //trebe dacase creeaza un director nou sa adaugi alea 3 fisiere in el
    //verifici daca exitsa directoru daca nu il adaugi
    //pot strge aia cu creeare downtown
    //functie separata de creere director
}

int main(int argc,char *argv[])
{
    if(argc!=7)
    {
        fprintf(stderr,"Invalid arguments");
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
            printf("has the role of %s\n",argv[2]);
        }

        if(strcmp(argv[2],"manager")==0)
        {
            printf("has the role of %s\n",argv[2]);
        }
    }

    if(strcmp(argv[3],"--user")==0)
    {
        printf("The name of the user is %s\n",argv[4]);
    }

    char *dirname = malloc(SIZE*sizeof(char));

    if(dirname==NULL)
    {
        fprintf(stderr,"Memory error\n");
        exit(-1);
    }

    strcpy(dirname,argv[6]);

    if(mkdir(dirname,0750)==0)
    {
        printf("Directory created successfully <3\n");
    }
    else
    {
        printf("Directory already exists:)\n");
    }
    chmod(dirname,0750);

    char path1[200];
    char path2[200];
    char path3[200];

    sprintf(path1,"%s/reports.dat",dirname);
    sprintf(path2,"%s/district.cfg",dirname);
    sprintf(path3,"%s/logged_district",dirname);

    //fiiser gol permisiune 0000
    createEmptyFile(path1);
    createEmptyFile(path2);
    createEmptyFile(path3);

    //setare permisiuni
    chmod(path1,0664);
    chmod(path2,0640);
    chmod(path3,0644);

    //comanda lis afisare detalii
    if(strcmp(argv[5],"--list") == 0)
    {
        listCommand(path1);
        free(dirname);
        return 0;
    }

    //check
    checkPermissions(argv[2], path1, path2, path3);

    //reports.dat
    FILE *f1 = openFile(path1,"ab");

    Report report;

    report.report_id = 1;
    strcpy(report.inspector_name, argv[4]);
    report.latitude = 45.75;
    report.longitude = 21.22;
    strcpy(report.issue_category,"road");
    report.severity_level = 3;
    report.timp = time(NULL);
    strcpy(report.description,"Groapa mare");

    fwrite(&report,sizeof(Report),1,f1);
    fclose(f1);

    // district.cfg
    FILE *f2 = openFile(path2,"w");
    fprintf(f2,"threshold=2\n");
    fclose(f2);

    f2 = openFile(path2,"r");

    int threshold;
    fscanf(f2,"threshold=%d",&threshold);
    fclose(f2);

    if(report.severity_level >= threshold)
        printf("ALERTA!\n");
    else
        printf("Nu este alerta\n");

    //logged_district
    FILE *f3 = openFile(path3,"a");
    printf("\n");
    printf("SE SCRIE IN LOG\n");
    fprintf(f3,"[%ld] %s %s %s %s\n",
            time(NULL), argv[2], argv[4], argv[5], argv[6]);

    fclose(f3);

    f3 = openFile(path3,"r");

    char linie[200];

    while(fgets(linie,sizeof(linie),f3) != NULL)
        printf("%s",linie);

    fclose(f3);

    free(dirname);

    return 0;
}
