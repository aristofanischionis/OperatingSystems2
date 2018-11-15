#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "./HeaderFiles/Records.h"

// char *fifo = "myfifo";

int InputDirector(int argc, char *argv[])
{
    // Decode prompt ./myfind –h Height –d Datafile -p Pattern -s
    int i = 1;
    int h = 1;
    int sflag = 0;
    char *datafile;
    char *pattern;
    pid_t pid;
    int status;
    MyRecord rec;
    long lSize;
    int numOfrecords;
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
    //root node
    char *paramsSM[argc + 1];

    paramsSM[0] = (char *)malloc(5);
    strcpy(paramsSM[0], "./splitterMerger");
    paramsSM[1] = (char *)malloc(strlen(datafile) + 1);
    strcpy(paramsSM[1], datafile);
    paramsSM[2] = (char *)malloc(2);
    paramsSM[3] = (char *)malloc(12);
    paramsSM[4] = (char *)malloc(strlen(pattern) + 1);
    strcpy(paramsSM[4], pattern);
    paramsSM[5] = (char *)malloc(12);
    sprintf(paramsSM[5], "%d", h);
    paramsSM[6] = (char *)malloc(20);
    strcpy(paramsSM[6], "FinalResults");
    // making the FinalResults fifo
    if (mkfifo(paramsSM[6], 0666) == -1)
    {
        perror(" Error creating the named pipe ");
        exit(1);
    }
    // check for the sflag
    if (sflag)
    {
        paramsSM[7] = (char *)malloc(3);
        strcpy(paramsSM[6], "-s");
        paramsSM[8] = NULL;
    }
    else
        paramsSM[7] = NULL;

    // forking the first SM
    if ((pid = fork()) == -1)
    {
        perror(" fork ");
        exit(1);
    }
    if (pid != 0)
    { // parent
        printf(" I am the parent process % d\n", getpid());
        if (wait(&status) != pid)
        { // check if child returns
            perror(" wait ");
            exit(1);
        }
        printf(" Child terminated with exit code %d\n", status >> 8);
    }
    else
    { //child
        paramsSM[2] = strcpy(paramsSM[2], "0");
        FILE *fpb;
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

        paramsSM[2] = strcpy(paramsSM[2], "0");
        sprintf(paramsSM[3], "%d", numOfrecords);

        printf(" I am the child process %d ", getpid());
        printf(" and will be replaced with ’ splitterMerger ’\n");
        execvp("./splitterMerger", paramsSM);
        exit(1);
    }

    exit(0);
}
