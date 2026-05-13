#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
//defines permission constants
//S_IRUSR -> macro for user read permission
//man 2 chmod
#include <unistd.h>
#include <signal.h>
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

//checks file permissions before opening the file
//uses stat() to read file information
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

//creates an empty file with 0000 permissions
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

//permission check -> see man 2 chmod
//checks access rights based on the current role
void checkPermissions(char *role, char *path1, char *path2, char *path3)
{
    if(strcmp(role,"inspector")==0)
    {
        if(!checkWithStat(path1,S_IRGRP) || !checkWithStat(path1,S_IWGRP))
        {
            printf("Inspector cannot access reports.dat\n");
            exit(1);
        }

        if(!checkWithStat(path2,S_IRGRP))
        {
            printf("Inspector cannot read district.cfg\n");
            exit(1);
        }

        if(!checkWithStat(path3,S_IROTH))
        {
            printf("Inspector cannot read logged_district\n");
            exit(1);
        }
    }
    else if(strcmp(role,"manager")==0)
    {
        if(!checkWithStat(path1,S_IRUSR) || !checkWithStat(path1,S_IWUSR))
        {
            printf("Manager cannot access reports.dat\n");
            exit(1);
        }

        if(!checkWithStat(path2,S_IRUSR) || !checkWithStat(path2,S_IWUSR))
        {
            printf("Manager cannot access district.cfg\n");
            exit(1);
        }

        if(!checkWithStat(path3,S_IRUSR) || !checkWithStat(path3,S_IWUSR))
        {
            printf("Manager cannot access logged_district\n");
            exit(1);
        }
    }
    else
    {
        printf("Invalid role\n");
        exit(1);
    }
}

void checkDirectoryPermission(char *dirname, char *role)
{
    struct stat st;

    if(stat(dirname,&st)==-1)
    {
        printf("District not found\n");
        exit(1);
    }

    if(strcmp(role,"manager")==0)
    {
        if(!(st.st_mode & S_IRUSR) ||
           !(st.st_mode & S_IWUSR) ||
           !(st.st_mode & S_IXUSR))
        {
            printf("Manager cannot access district directory\n");
            exit(1);
        }
    }
    else
    {
        if(!(st.st_mode & S_IRGRP) ||
           !(st.st_mode & S_IXGRP))
        {
            printf("Inspector cannot access district directory\n");
            exit(1);
        }
    }
}

void writeLog(char *dirname, char *role, char *user, char *command, time_t timestamp)
{
    char path[200];
    struct tm *timeinfo;
    char timeString[50];

    if(strcmp(role,"manager")!=0)
        return;

    sprintf(path,"%s/logged_district", dirname);

    FILE *f = fopen(path, "a");

    if(f == NULL)
    {
        printf("Cannot open logged_district\n");
        return;
    }

    timeinfo = localtime(&timestamp);
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeinfo);

    fprintf(f, "[%s] %s %s %s\n",
            timeString, role, user, command);

    fclose(f);
}

//converts permission bits to string
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
    //W-write permission for group
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

//prints report details
//used by both list and view commands
void printReport(Report report)
{
    printf("Report ID: %d\n",report.report_id);
    printf("Inspector Name: %s\n",report.inspector_name);
    printf("Latitude: %.2f\n",report.latitude);
    printf("Longitude: %.2f\n",report.longitude);
    printf("Issue Category: %s\n",report.issue_category);
    printf("Severity Level: %d\n",report.severity_level);
    printf("Timestamp: %s",ctime(&report.timp));
    printf("Description: %s\n",report.description);
    printf("\n");
}

//checks for dangling symbolic links
void checkSymlink(char *dirname)
{
    char linkname[200];
    char target[200];
    struct stat st;
    int n;

    sprintf(linkname,"active_reports-%s",dirname);

    if(lstat(linkname,&st)==-1)
    {
        printf("No symlink found for district %s\n",dirname);
        return;
    }

    if(S_ISLNK(st.st_mode))
    {
        n = readlink(linkname,target,sizeof(target)-1);

        if(n==-1)
        {
            printf("Cannot read symlink\n");
            return;
        }

        target[n]='\0';

        if(stat(target,&st)==-1)
        {
            printf("Warning: dangling link %s -> %s\n",linkname,target);
        }
        else
        {
            printf("Symlink OK: %s -> %s\n",linkname,target);
        }
    }
}

//list command
//example: ./prog --role inspector --user ana --list downtown
//this is used to display all reports from a district
void listCommand(char *dirname)
{
    char fisier[200];
    struct stat st;
    char string[10];

    checkSymlink(dirname);

    sprintf(fisier,"%s/reports.dat",dirname);

    if(stat(fisier,&st)==-1)
    {
        printf("District or reports.dat not found\n");
        return;
    }

    permissionToString(st.st_mode,string);

    printf("District: %s\n",dirname);
    printf("Permissions: %s\n",string);
    printf("Size: %ld bytes\n",st.st_size);
    printf("Last modified: %s",ctime(&st.st_mtime));

    FILE *f = fopen(fisier,"rb");

    if(f==NULL)
    {
        printf("Cannot open reports.dat\n");
        return;
    }

    Report report;
    int count = 0;

    printf("\nAll Reports:\n\n");

    while(fread(&report,sizeof(Report),1,f)==1)
    {
        count++;
        printReport(report);

    }

    printf("Total Reports: %d\n",count);

    fclose(f);
}

//view command
void viewCommand(char *dirname, int wantedId)
{
    char fisier[200];
    FILE *f;
    Report report;
    int found = 0;

    sprintf(fisier,"%s/reports.dat",dirname);

    f = fopen(fisier,"rb");

    if(f==NULL)
    {
        printf("Cannot open reports.dat\n");
        return;
    }

    while(fread(&report,sizeof(Report),1,f)==1)
    {
        if(report.report_id == wantedId)
        {
            printReport(report);
            found = 1;
            break;
        }
    }

    if(found==0)
        printf("Report not found\n");

    fclose(f);
}

//additional permission check for reports.dat
void ensureReportsPermissions(char *path)
{
    struct stat st;

    if(stat(path,&st)==-1)
    {
        printf("Cannot read reports.dat info\n");
        return;
    }

    if((st.st_mode & 0777) != 0664)
    {
        chmod(path,0664);
        printf("reports.dat permissions changed to 664\n");
    }
    else
    {
        printf("reports.dat already has 664 permissions\n");
    }
}

int directoryExists(char *dirname)
{
    struct stat st;

    if(stat(dirname, &st) == 0 && S_ISDIR(st.st_mode))
        return 1;

    return 0;
}

void createDistrict(char *dirname)
{
    char path1[200];
    char path2[200];
    char path3[200];

    if(directoryExists(dirname))
    {
        printf("District already exists.\n");
        return;
    }

    if(mkdir(dirname,0750)==-1)
    {
        perror("mkdir");
        return;
    }

    sprintf(path1,"%s/reports.dat",dirname);
    sprintf(path2,"%s/district.cfg",dirname);
    sprintf(path3,"%s/logged_district",dirname);

    createEmptyFile(path1);
    createEmptyFile(path2);
    createEmptyFile(path3);

    FILE *cfg = fopen(path2,"w");

    if(cfg!=NULL)
    {
        fprintf(cfg,"threshold=2\n");
        fclose(cfg);
    }

    chmod(path1,0664);
    chmod(path2,0640);
    chmod(path3,0644);

    printf("District created successfully!\n");
}

//creates or updates symbolic links
void updateSymlink(char *dirname)
{
    char linkname[200];
    char target[200];
    struct stat st;

    sprintf(linkname,"active_reports-%s",dirname);
    sprintf(target,"%s/reports.dat",dirname);

    if(lstat(linkname,&st)==0)
    {
        unlink(linkname);
    }

    if(symlink(target,linkname)==-1)
    {
        perror("symlink");
        return;
    }

    printf("Symlink updated: %s -> %s\n",linkname,target);
}

//add command

//in add, check if the district directory exists
//check the three required files
//check permissions
//create or update the symbolic link
//if the district does not exist, create it with the required files
//separate function used for district creation
void addReport(char *dirname, char *user, char *role)
{
    char path1[200];
    char path2[200];
    char path3[200];

    createDistrict(dirname);

    sprintf(path1,"%s/reports.dat",dirname);
    sprintf(path2,"%s/district.cfg",dirname);
    sprintf(path3,"%s/logged_district",dirname);

    checkDirectoryPermission(dirname,role);
    checkPermissions(role,path1,path2,path3);

    if(access(path1,F_OK)!=0)
        createEmptyFile(path1);

    ensureReportsPermissions(path1);
    updateSymlink(dirname);

    if(access(path2,F_OK)!=0)
    {
        createEmptyFile(path2);
        chmod(path2,0640);
    }

    if(access(path3,F_OK)!=0)
    {
        createEmptyFile(path3);
        chmod(path3,0644);
    }

    Report report;
    Report x;

    FILE *temp = fopen(path1,"rb");
    int id = 1;

    if(temp != NULL)
    {
        while(fread(&x,sizeof(Report),1,temp)==1)
        {
            if(x.report_id >= id)
                id = x.report_id + 1;
        }

        fclose(temp);
    }

    report.report_id = id;

    strcpy(report.inspector_name,user);
    printf("Latitude: ");
    scanf("%f",&report.latitude);

    printf("Longitude: ");
    scanf("%f",&report.longitude);

    printf("Category: ");
    scanf("%29s",report.issue_category);

    printf("Severity (1-3): ");
    scanf("%d",&report.severity_level);

    if(report.severity_level < 1 || report.severity_level > 3)
    {
        printf("Invalid severity level\n");
        return;
    }

    printf("Description: ");
    scanf(" %99[^\n]",report.description);

    report.timp = time(NULL);

    int fd = open(path1,O_WRONLY | O_APPEND);

    if(fd == -1)
    {
        printf("Cannot open reports.dat\n");
        return;
    }

    if(write(fd,&report,sizeof(Report)) != sizeof(Report))
    {
        printf("Write error\n");
        close(fd);
        return;
    }

    close(fd);

    printf("Report added successfully!\n");

    writeLog(dirname,role,user,"add", report.timp);

    FILE *cfg = fopen(path2,"r");
    int threshold = 0;

    if(cfg != NULL)
    {
        fscanf(cfg,"threshold=%d",&threshold);
        fclose(cfg);

        if(report.severity_level >= threshold)
            printf("ALERT! Severity >= threshold\n");
        else
            printf("No alert\n");
    }
   //flag used to check if the monitor notification was successful
    int contorMonitor=0;
    FILE *m=fopen(".monitor_pid","r");
    if(m==NULL)
    {
        perror("error opening hidden file");

    }
    else
    {
        int monitor_pid;
        //if the PID is read successfully
        if(fscanf(m,"%d",&monitor_pid)==1)
        {
            ///kill()-> sends a signal, it does not kill the process here
            //0 means the signal was sent successfully
            if(kill(monitor_pid,SIGUSR1)==0)
            {
                contorMonitor=1;
            }
        }
        fclose(m);
    }
    if(contorMonitor==0)
    {
        writeLog(dirname,role,user,"Monitor PID could not be found or signal could not be sent", report.timp);
    }
    else
    {
        writeLog(dirname,role,user,"Monitor notified successfully", report.timp);

    }

}

//remove report command
void removeReport(char *dirname, int wantedId, char *role, char *user)
{
    char fisier[200];
    //make sure the symbolic link is also removed if needed
    struct stat st;
    char linkname[200];
    sprintf(linkname,"active_reports-%s",dirname);
    int fd;
    Report report;
    int found = 0;
    int index = 0;
    int total;
    int i;

    if(strcmp(role,"manager")!=0)
    {
        printf("Access denied. Manager only.\n");
        return;
    }

    sprintf(fisier,"%s/reports.dat",dirname);

    fd = open(fisier,O_RDWR);

    if(fd==-1)
    {
        printf("Cannot open reports.dat\n");
        return;
    }

    while(read(fd,&report,sizeof(Report)) == sizeof(Report))
    {
        if(report.report_id == wantedId)
        {
            found = 1;
            break;
        }

        index++;
    }

    if(found==0)
    {
        printf("Report not found\n");
        close(fd);
        return;
    }

    total = lseek(fd,0,SEEK_END) / sizeof(Report);

    for(i=index+1;i<total;i++)
    {
        Report temp;

        lseek(fd,i*sizeof(Report),SEEK_SET);
        read(fd,&temp,sizeof(Report));

        lseek(fd,(i-1)*sizeof(Report),SEEK_SET);
        write(fd,&temp,sizeof(Report));
    }

    ftruncate(fd,(total-1)*sizeof(Report));


    if(stat(fisier,&st)==0 && st.st_size==0)
    {
        unlink(linkname);
        printf("Symlink removed because reports.dat is empty\n");
    }

    close(fd);

    printf("Report removed successfully!\n");

    writeLog(dirname,role,user,"remove_report",time(NULL));
}

//helper function for removing a district
//unlink should be done only after the directory was removed successfully
//remove_district command
 void remove_district(char*idDistrict,char *role)
 {
     checkDirectoryPermission(idDistrict,role);
     int pid=fork();
     if(pid<0)
     {
         printf("Error while creating child process\n");
         exit(0);
     }
     //execute external command: rm -rf <district_directory>
     //pid == 0 means this is the child process
     else if(pid==0)
     {
         printf("Child process started\n");
         char *arguments[]={"rm","-rf",idDistrict,NULL};
         execvp("rm",arguments);
         exit(0);
     }
     //execlp -> list of arguments
     //execvp -> vector of arguments
     //pid > 0 means this is the parent process
     else if(pid>0)
     {
         printf("Parent process is waiting for child process\n");
         int status_ptr;
         //WUNTRACED-also return if a child has stopped
         if(waitpid(pid,&status_ptr,WUNTRACED)==-1)
             //-1 meaning wait for any child process.
         {
             printf("Error while waiting for child process\n");
                 exit(-1);
         }
          char linkname[200];
          sprintf(linkname,"active_reports-%s",idDistrict);

          if(unlink(linkname)==0)
          {
             printf("Symbolic link removed\n");
          }

     }

}

//update_threshold function
void updateThreshold(char *dirname,int value,char *role,char *user)
{
    char fisier[200];
    struct stat st;
    FILE *f;

    if(strcmp(role,"manager")!=0)
    {
        printf("Access denied. Manager only.\n");
        return;
    }

    sprintf(fisier,"%s/district.cfg",dirname);

    if(stat(fisier,&st)==-1)
    {
        printf("district.cfg not found\n");
        return;
    }

    if((st.st_mode & 0777) != 0640)
    {
        printf("district.cfg permissions invalid. Expected 640\n");
        return;
    }

    f=fopen(fisier,"w");

    if(f==NULL)
    {
        printf("Cannot open district.cfg\n");
        return;
    }

    fprintf(f,"threshold=%d\n",value);

    fclose(f);

    printf("Threshold updated successfully!\n");

    writeLog(dirname,role,user,"update_threshold",time(NULL));
}

int parse_condition(const char *input, char *field, char *op, char *value)
{
    char copie[200];
    char *p1;
    char *p2;

    strcpy(copie,input);

    p1 = strchr(copie,':');

    if(p1==NULL)
        return 0;

    *p1='\0';

    p2 = strchr(p1+1,':');

    if(p2==NULL)
        return 0;

    *p2='\0';

    strcpy(field,copie);
    strcpy(op,p1+1);
    strcpy(value,p2+1);

    return 1;
}

int match_condition(Report *r, const char *field, const char *op, const char *value)
{
    if(strcmp(field,"severity")==0)
    {
        int v = atoi(value);

        if(strcmp(op,"==")==0) return r->severity_level == v;
        if(strcmp(op,"!=")==0) return r->severity_level != v;
        if(strcmp(op,"<")==0) return r->severity_level < v;
        if(strcmp(op,"<=")==0) return r->severity_level <= v;
        if(strcmp(op,">")==0) return r->severity_level > v;
        if(strcmp(op,">=")==0) return r->severity_level >= v;
    }

    if(strcmp(field,"category")==0)
    {
        if(strcmp(op,"==")==0) return strcmp(r->issue_category,value)==0;
        if(strcmp(op,"!=")==0) return strcmp(r->issue_category,value)!=0;
    }

    if(strcmp(field,"inspector")==0)
    {
        if(strcmp(op,"==")==0) return strcmp(r->inspector_name,value)==0;
        if(strcmp(op,"!=")==0) return strcmp(r->inspector_name,value)!=0;
    }

    if(strcmp(field,"timestamp")==0)
    {
        time_t v = atol(value);

        if(strcmp(op,"==")==0) return r->timp == v;
        if(strcmp(op,"!=")==0) return r->timp != v;
        if(strcmp(op,"<")==0) return r->timp < v;
        if(strcmp(op,"<=")==0) return r->timp <= v;
        if(strcmp(op,">")==0) return r->timp > v;
        if(strcmp(op,">=")==0) return r->timp >= v;
    }

    return 0;
}

//filter command
void filterCommand(char *dirname, int conditionCount, char *conditions[])
{
    char fisier[200];
    int fd;
    Report report;

    sprintf(fisier,"%s/reports.dat",dirname);

    fd = open(fisier,O_RDONLY);

    if(fd==-1)
    {
        printf("Cannot open reports.dat\n");
        return;
    }

    while(read(fd,&report,sizeof(Report)) == sizeof(Report))
    {
        int ok = 1;

        for(int i=0;i<conditionCount;i++)
        {
            char field[50];
            char op[10];
            char value[100];

            if(parse_condition(conditions[i],field,op,value)==0)
            {
                printf("Invalid condition: %s\n",conditions[i]);
                ok = 0;
                break;
            }

            if(match_condition(&report,field,op,value)==0)
            {
                ok = 0;
                break;
            }
        }

        if(ok==1)
        {
            printReport(report);
        }
    }

    close(fd);
}


int main(int argc,char *argv[])
{
    if(argc < 7)
    {
        printf("Invalid arguments\n");
        return 1;
    }

    if(strcmp(argv[1],"--role")!=0 || strcmp(argv[3],"--user")!=0)
    {
        printf("Bad arguments\n");
        return 1;
    }

    char *dirname = malloc(SIZE);

    if(dirname==NULL)
    {
        printf("Memory error\n");
        return 1;
    }

    strcpy(dirname,argv[6]);

    if(strcmp(argv[5],"--add")==0)
    {
        addReport(dirname,argv[4],argv[2]);
        free(dirname);
        return 0;
    }

    if(strcmp(argv[5],"--list")==0)
    {
        char path1[200];
        char path2[200];
        char path3[200];

        sprintf(path1,"%s/reports.dat",dirname);
        sprintf(path2,"%s/district.cfg",dirname);
        sprintf(path3,"%s/logged_district",dirname);

        checkDirectoryPermission(dirname,argv[2]);
        checkPermissions(argv[2],path1,path2,path3);

        listCommand(dirname);

        if(strcmp(argv[2],"manager")==0)
            writeLog(dirname,argv[2],argv[4],"list",time(NULL));

        free(dirname);
        return 0;
    }

    if(strcmp(argv[5],"--view")==0)
    {
        if(argc < 8)
        {
            printf("Missing report id\n");
            free(dirname);
            return 1;
        }

        char path1[200];
        char path2[200];
        char path3[200];

        sprintf(path1,"%s/reports.dat",dirname);
        sprintf(path2,"%s/district.cfg",dirname);
        sprintf(path3,"%s/logged_district",dirname);

        checkDirectoryPermission(dirname,argv[2]);
        checkPermissions(argv[2],path1,path2,path3);

        viewCommand(dirname,atoi(argv[7]));

        if(strcmp(argv[2],"manager")==0)
            writeLog(dirname,argv[2],argv[4],"view",time(NULL));

        free(dirname);
        return 0;
    }

    if(strcmp(argv[5],"--remove_report")==0)
    {
        if(argc < 8)
        {
            printf("Missing report id\n");
            free(dirname);
            return 1;
        }

        char path1[200];
        char path2[200];
        char path3[200];

        sprintf(path1,"%s/reports.dat",dirname);
        sprintf(path2,"%s/district.cfg",dirname);
        sprintf(path3,"%s/logged_district",dirname);

        checkDirectoryPermission(dirname,argv[2]);
        checkPermissions(argv[2],path1,path2,path3);

        removeReport(dirname,atoi(argv[7]),argv[2],argv[4]);

        free(dirname);
        return 0;
    }

    if(strcmp(argv[5],"--update_threshold")==0)
    {
        if(argc < 8)
        {
            printf("Missing threshold value\n");
            free(dirname);
            return 1;
        }

        checkDirectoryPermission(dirname,argv[2]);

        updateThreshold(dirname,atoi(argv[7]),argv[2],argv[4]);

        free(dirname);
        return 0;
    }

    if(strcmp(argv[5],"--filter")==0)
    {
        char path1[200];
        char path2[200];
        char path3[200];

        sprintf(path1,"%s/reports.dat",dirname);
        sprintf(path2,"%s/district.cfg",dirname);
        sprintf(path3,"%s/logged_district",dirname);

        checkDirectoryPermission(dirname,argv[2]);
        checkPermissions(argv[2],path1,path2,path3);

        filterCommand(dirname,argc-7,&argv[7]);

        if(strcmp(argv[2],"manager")==0)
            writeLog(dirname,argv[2],argv[4],"filter",time(NULL));

        free(dirname);
        return 0;
    }
     if(strcmp(argv[5],"--remove_district")==0)
    {

        checkDirectoryPermission(dirname,argv[2]);

        remove_district(dirname,argv[2]);
        free(dirname);
        return 0;
    }

    printf("Unknown command\n");
    free(dirname);
    return 1;
}

