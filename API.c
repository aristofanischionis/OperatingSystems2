#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include "./HeaderFiles/Records.h"

int SignalsReceived = 0;

void handler()
{
    signal(SIGUSR2, handler);
    // printf("DADDY: I have received a SIGUSR2, FROM MY KIDDO\n");
    SignalsReceived++;
}

double MIN(double array[], int num){
    double min = 10000.0;
    for(int i = 0; i<num; i++){
        if(array[i] < min){
            min = array[i];
        }
    }
    return min;
}
double MAX(double array[], int num){
    double max = 0.0;
    for(int i = 0; i<num; i++){
        if(array[i] > max){
            max = array[i];
        }
    }
    return max;
}
double Average(double array[], int num){
    double sum= 0.0;
    for(int i=0 ; i < num;i++){
        sum += array[i];
    }
    return sum/num;
}
void statistics(double leafs[], double SMs[], int leafnum, int smnum){
    // calculate everything
    double minl = MIN(leafs, leafnum);
    double maxl = MAX(leafs, leafnum);
    double averl = Average(leafs, leafnum);
    double minsm = MIN(SMs, smnum);
    double maxsm = MAX(SMs, smnum);
    double aversm = Average(SMs, smnum);
    // print everything
    printf("-----------------STATISTICS STARTING HERE-----------------\n");
    printf("Searchers min time is %f, max time is %f, average time is %f\n", minl, maxl, averl);
    printf("Splitter/Mergers min time is %f, max time is %f, average time is %f\n", minsm, maxsm, aversm);
    
}

int ReadDatafile(char *datafile){
    MyRecord rec;
    long lSize;
    FILE *fpb;
    int numOfrecords;
    fpb = fopen(datafile, "rb");
    if (fpb == NULL)
    {
        printf("Cannot open binary file\n");
        return 1;
    }
    // check number of records
    fseek(fpb, 0, SEEK_END);
    lSize = ftell(fpb);
    numOfrecords = (int)lSize / sizeof(rec);
    fclose(fpb);
    return numOfrecords;
}

int InputDirector(int argc, char *argv[])
{
    // start timer
    struct timeval t0,t1;
    gettimeofday(&t0, NULL);
    // Decode prompt ./myfind –h Height –d Datafile -p Pattern -s
    int i = 1, h = 1, sum = 0, sflag = 0, numOfrecords;
    int leafnum = 1,smnum, height, counteri = 0, counterj = 0, num = 10;
    char *datafile;
    char *pattern;
    pid_t pid;
    MyRecord rec;
    const char s[2] = " ";
    char *token;
    while (i < argc)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            if (i < argc - 1)
            {
                if (strcmp(argv[i + 1], "-d") == 0)
                {
                    printf("Height not given after -h flag\n");
                    break; // giving the default value 1 to height.
                }
                printf("Height is: %s\n", argv[i + 1]);
                h = atoi(argv[i + 1]);
                if (h > 5 || h < 1)
                {
                    printf("Acceptable heights are from 1-5, you gave %d, I will put the default h=1\n", h);
                    return 1;
                    h = 1;
                }
                break;
            }
            else
            {
                printf("Height not given after -h flag\n");
                break;
            }
        }
        i++;
    }
    i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            if (i < argc - 1)
            {
                if (strcmp(argv[i + 1], "-p") == 0)
                {
                    printf("Datafile not given after -d flag, program will terminate now\n");
                    return 1;
                }
                printf("Datafile is: %s\n", argv[i + 1]);
                datafile = (char *)malloc(strlen(argv[i + 1]) * sizeof(char) + 1);
                strcpy(datafile, argv[i + 1]);
                break;
            }
            else
            {
                printf("Datafile not given after -d flag, program will terminate now\n");
                return 1;
            }
        }
        i++;
    }
    i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            if (i < argc - 1)
            {
                if (strcmp(argv[i + 1], "-s") == 0)
                {
                    printf("Pattern not given after -p flag, program will terminate now\n");
                    return 1;
                }
                printf("Pattern is: %s\n", argv[i + 1]);
                pattern = (char *)malloc(strlen(argv[i + 1]) * sizeof(char) + 1);
                strcpy(pattern, argv[i + 1]);
                break;
            }
            else
            {
                printf("Pattern not given after -p flag, program will terminate now\n");
                return 1;
            }
        }
        i++;
    }
    i = 1;
    while (i < argc)
    {
        if (strcmp(argv[i], "-s") == 0)
        {
            printf("s flag is now enabled\n");
            sflag = 1;
        }
        i++;
    }
    // preparing for statistics
    // h determines how many leafs and sm there will be
    height = h;
    while(height)
    {
        height--;
        leafnum = 2 * leafnum;
    }
    smnum = leafnum - 1;
    double leaftimes[leafnum];
    double smtimes[smnum];
    
    //root node making the parameters for the SM
    if(sflag) num = 12;
    char *paramsSM[num];
    paramsSM[0] = (char *)malloc(20);
    paramsSM[1] = (char *)malloc(strlen(datafile) + 1);
    paramsSM[2] = (char *)malloc(2); // rangebeg
    paramsSM[3] = (char *)malloc(12); // numof records
    paramsSM[4] = (char *)malloc(strlen(pattern) + 1); // pattern
    paramsSM[5] = (char *)malloc(12); // height
    paramsSM[6] = (char *)malloc(12); // initial height
    paramsSM[7] = (char *)malloc(20); // file for results
    paramsSM[8] = (char *)malloc(12);
    strcpy(paramsSM[0], "./splitterMerger"); // name of exec
    strcpy(paramsSM[1], datafile); // datafile
    //
    //
    strcpy(paramsSM[4], pattern);
    sprintf(paramsSM[5], "%d", h);
    sprintf(paramsSM[6], "%d", h);
    strcpy(paramsSM[7], "FinalResults");
    // giving my pid to the leafs through the SMs in order to receive the signals
    sprintf(paramsSM[8], "%d", getpid());
    // time for the binary file
    numOfrecords = ReadDatafile(datafile);
    // setting up 2,3 params
    if (sflag)
    {
        paramsSM[9] = (char *)malloc(3);
        paramsSM[10] = (char *)malloc(12);
        strcpy(paramsSM[2], "1");
        sprintf(paramsSM[3], "%d", leafnum);
        strcpy(paramsSM[9], "-s");
        sprintf(paramsSM[10], "%d", numOfrecords);
        paramsSM[11] = NULL;
    }
    else
    {
        strcpy(paramsSM[2], "0");
        sprintf(paramsSM[3], "%d", numOfrecords);
        paramsSM[9] = NULL;
    }
    
    // making the FinalResults fifo
    if (mkfifo(paramsSM[7], 0666) == -1)
    {
        perror(" Error creating the named pipe ");
        exit(1);
    }
    // setting the signal receiver
    signal(SIGUSR2, handler);

    // forking the first SM
    if ((pid = fork()) == -1)
    {
        perror(" fork ");
        exit(1);
    }
    if (pid != 0)
    { // parent
        int fd, nread = 0;
        if ((fd = open(paramsSM[7], O_RDONLY)) == -1)
        {
            perror("fifo open error");
            exit(1);
        }
        FILE *final;
        final = fopen("ResultsNotSorted", "w");
        while ((nread = read(fd, &rec, sizeof(rec)) > 0))
        {
            if (rec.AM == -1)
            {
                // printf("----------->I have to read statistics\n");
                char stat[25];
                nread = read(fd, stat, sizeof(stat));
                // printf("FINAL stat is %s\n", stat);
                
                token = strtok(stat, s);
                if (strcmp(token, "SM") == 0)
                {
                    // time for a SM
                    // printf(" SM stat found %s", strtok(NULL, s));
                    smtimes[counteri] = atof(strtok(NULL, s));
                    counteri++;
                }
                else if (strcmp(token, "Searcher") == 0)
                {
                    // time for a Searcher
                    // printf(" Searcher stat found %s", strtok(NULL, s));
                    leaftimes[counterj] = atof(strtok(NULL, s));
                    counterj++;
                }
                continue;
            }
            sum++;
            // write data to a file in order to fork a new process and call sort on it
            fprintf(final, "%ld %s %s  %s %d %s %s %-9.2f\n", rec.AM, rec.LastName, rec.FirstName,
                    rec.Street, rec.HouseID, rec.City, rec.postcode,
                    rec.salary);
        }

        fclose(final);
        // remove file
        remove(paramsSM[7]);
        // fork to call the sort
        pid_t pidSort;
        if ((pidSort = fork()) == -1)
        {
            perror(" fork ");
            exit(1);
        }
        if (pidSort == 0)
        {
            //child
            // printf(" I am the child process %d ", getpid());
            // printf(" and will be replaced with ’ Sort ’\n");
            char *params[6];
            params[0] = "sort";
            params[1] = "-k";
            params[2] = "1";
            params[3] = "-g";
            params[4] = "ResultsNotSorted";
            params[5] = NULL;
            execvp("sort", params);
        }
        //parent
        // waits for kids to finish
        pid_t wpid;
        int status = 0;
        while ((wpid = wait(&status)) > 0);
        // rm the ResultsNotSorted file
        remove("ResultsNotSorted");
        // sum of records read
        printf("I have read %d records from file\n", sum);
        // printf signals
        printf("I am the parent process and I have received %d signals from my kids\n", SignalsReceived);
        // print statistics
        statistics(leaftimes, smtimes, leafnum, smnum);
        // turnaround time
        gettimeofday(&t1, NULL);
        double turnaround = (double) (t1.tv_usec - t0.tv_usec) / 1000000 + (double) (t1.tv_sec - t0.tv_sec);
        printf("Turnaround time is %f seconds\n", turnaround);

        printf("-----------------STATISTICS END HERE----------------------\n");
       
    }
    else
    { //child
        // printf(" I am the child process %d ", getpid());
        // printf(" and will be replaced with ’ splitterMerger ’\n");
        execvp("./splitterMerger", paramsSM);
    }

    exit(0);
}
