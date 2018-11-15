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

void spawnKids(
    char *argv[],
    char *datafile,
    int rangeBeg, 
    char *pattern, 
    int numOfrecords, 
    int h, 
    int sflag, 
    char *MyResults)
{
    char *params[7];
    int num1, num2, myfd;
    pid_t pid1, pid2;
    int status;
    params[0] = (char *)malloc(5);
    strcpy(params[0], "./leaf");
    params[1] = (char *)malloc(strlen(datafile) + 1);
    strcpy(params[1], datafile);
    params[2] = (char *)malloc(12);
    params[3] = (char *)malloc(12);
    params[4] = (char *)malloc(strlen(pattern) + 1);
    strcpy(params[4], pattern);
    params[5] = (char *)malloc(20);
    params[6] = NULL;
    num1 = numOfrecords / 2;
    num2 = numOfrecords - num1;
    // to handle the case of odd numbersof records
    //  opening my fifo to write the results from kids
    if ((myfd = open(MyResults, O_WRONLY)) == -1)
    {
        perror("fifo open error");
        exit(1);
    }
    //
    char KidResults1[20];
    char KidResults2[20];
    char stat1[20];
    char stat2[20];
    // giving name to each fifo
    sprintf(KidResults1, "%s%d", "KidResults1_", getpid());
    sprintf(KidResults2, "%s%d", "KidResults2_", getpid());
    //
    // making the fifos for my kids
    if (mkfifo(KidResults1, 0666) == -1)
    {
        perror(" Error creating the named pipe ");
        exit(1);
    }
    if (mkfifo(KidResults2, 0666) == -1)
    {
        perror(" Error creating the named pipe ");
        exit(1);
    }
    // placing the timer
    clock_t begin = clock();
    // forking the kids
    if ((pid1 = fork()) == -1)
    {
        perror(" fork ");
        exit(1);
    }
    if (pid1 != 0)
    { // parent
        printf(" I am the parent process % d\n", getpid());
        if ((pid2 = fork()) == -1)
        {
            perror(" fork ");
            exit(1);
        }
        if (pid2 != 0)
        { // parent
            printf(" I am the parent process % d\n", getpid());
            // reading from fifo 2
            printf("I am %s and now i'm gonna read from my pipe\n", KidResults2);
            int fd, nread = 0;
            MyRecord rec;
            if ((fd = open(KidResults2, O_RDONLY)) == -1)
            {
                perror("fifo open error");
                exit(1);
            }

            while ((nread = read(fd, &rec, sizeof(rec)) > 0))
            {
                if (rec.AM == -1)
                {
                    printf("I just read end");
                    break;
                }

                printf("%ld %s %s  %s %d %s %s %-9.2f\n",
                       rec.AM, rec.LastName, rec.FirstName,
                       rec.Street, rec.HouseID, rec.City, rec.postcode,
                       rec.salary);
                // write this result in my dad's pipe
                if (write(myfd, &rec, sizeof(rec)) == -1)
                {
                    perror(" Error in Writing in pipe\n");
                    exit(2);
                }
            }

            nread = read(fd, stat2, sizeof(stat2));
            printf("time for kid 2 is %s\n", stat2);
            // write this time to my dad's pipe
            if (write(myfd, stat2, sizeof(stat2)) == -1)
            {
                perror(" Error in Writing in pipe\n");
                exit(2);
            }
            if (wait(&status) != pid2)
            { // check if child returns
                perror(" wait ");
                exit(1);
            }
            printf(" Child terminated with exit code %d\n", status >> 8);
            //
            //
        }
        else
        { //child 2
            if (sflag)
            {
                //have to think about it
            }
            else
            {
                sprintf(params[2], "%d", rangeBeg + num1);
                sprintf(params[3], "%d", num2);
            }
            strcpy(params[5], KidResults2);
            printf(" I am the child process %d ", getpid());
            printf(" and will be replaced with ’ leaf ’\n");
            execvp("./leaf", params);
            exit(1);
        }
        // reading from fifo 1
        printf("I am %s and now i'm gonna read from my pipe\n", KidResults1);
        int fd, nread = 0;
        MyRecord rec;

        if ((fd = open(KidResults1, O_RDONLY)) == -1)
        {
            perror("fifo open error");
            exit(1);
        }
        while ((nread = read(fd, &rec, sizeof(rec)) > 0))
        {
            if (rec.AM == -1)
            {
                printf("I just read end");
                break;
            }

            printf("%ld %s %s  %s %d %s %s %-9.2f\n",
                   rec.AM, rec.LastName, rec.FirstName,
                   rec.Street, rec.HouseID, rec.City, rec.postcode,
                   rec.salary);
            if (write(myfd, &rec, sizeof(rec)) == -1)
            {
                perror(" Error in Writing in pipe\n");
                exit(2);
            }
        }

        nread = read(fd, stat1, sizeof(stat1));
        printf("time for kid 1 is %s\n", stat1);
        if (write(myfd, stat1, sizeof(stat1)) == -1)
        {
            perror(" Error in Writing in pipe\n");
            exit(2);
        }
        // end of timer
        clock_t end = clock();
        rec.AM = -1;
        //
        if (write(myfd, &rec, sizeof(rec)) == -1)
        { // to specify that statistics are coming for the dad
            perror(" Error in Writing in pipe\n");
            exit(2);
        }
        char tobewritten[25];
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("PID %d needed %f\n", getpid(), time_spent);
        sprintf(tobewritten, "SM %f", time_spent);
        if ((write(myfd, tobewritten, sizeof(tobewritten))) == -1)
        {
            perror(" Error at Writing in pipe\n");
            exit(2);
        }
        exit(0);

        // if( wait(&status) != pid1 ){ // check if child returns
        //     perror( " wait "); exit (1) ; }
        // printf(" Child terminated with exit code %d\n" , status >> 8) ;
    }
    else
    { //child 1
        if (sflag)
        {
            //have to think about it
        }
        else
        {
            params[2] = strcpy(params[2], argv[2]);
            sprintf(params[3], "%d", num1);
        }
        strcpy(params[5], KidResults1);
        printf(" I am the child process %d ", getpid());
        printf(" and will be replaced with ’ leaf ’\n");
        execvp("./leaf", params);
        exit(1);
    }

    exit(0);
}

void spawnSMs(
    int argc,
    char *argv[],
    char *datafile, 
    int rangeBeg,
    char *pattern,
    int numOfrecords,
    int h,
    int sflag)
{
    char *paramsSM[argc];
    int num1, num2;

    pid_t pid1, pid2;
    int status;
    paramsSM[0] = (char *)malloc(5);
    strcpy(paramsSM[0], "./splitterMerger");
    paramsSM[1] = (char *)malloc(strlen(datafile) + 1);
    strcpy(paramsSM[1], datafile);
    paramsSM[2] = (char *)malloc(12);
    paramsSM[3] = (char *)malloc(12);
    paramsSM[4] = (char *)malloc(strlen(pattern) + 1);
    strcpy(paramsSM[4], pattern);
    paramsSM[5] = (char *)malloc(12);
    sprintf(paramsSM[5], "%d", h - 1);
    paramsSM[6] = (char *)malloc(20); // namedpipe for results
    if (sflag)
    {
        paramsSM[7] = (char *)malloc(3);
        strcpy(paramsSM[6], "-s");
        paramsSM[8] = NULL;
    }
    else
        paramsSM[7] = NULL;

    num1 = numOfrecords / 2;
    num2 = numOfrecords - num1;
    // to handle the case of odd numbersof records

    char SMResults1[20];
    char SMResults2[20];
    // giving name to each fifo
    sprintf(SMResults1, "%s%d", "SMResults1_", getpid());
    sprintf(SMResults2, "%s%d", "SMResults2_", getpid());
    //
    // making the fifos for my kids
    if (mkfifo(SMResults1, 0666) == -1)
    {
        perror(" Error creating the named pipe ");
        exit(1);
    }
    if (mkfifo(SMResults2, 0666) == -1)
    {
        perror(" Error creating the named pipe ");
        exit(1);
    }

    //forking the kids
    if ((pid1 = fork()) == -1)
    {
        perror(" fork ");
        exit(1);
    }
    if (pid1 != 0)
    { // parent
        printf(" I am the parent process % d\n", getpid());

        if ((pid2 = fork()) == -1)
        {
            perror(" fork ");
            exit(1);
        }
        if (pid2 != 0)
        { // parent
            printf(" I am the parent process % d\n", getpid());
            if (wait(&status) != pid2)
            { // check if child returns
                perror(" wait ");
                exit(1);
            }
            printf(" Child terminated with exit code %d\n", status >> 8);
        }
        else
        { //child 2
            if (sflag)
            {
                //have to think about it
            }
            else
            {
                sprintf(paramsSM[2], "%d", rangeBeg + num1);
                sprintf(paramsSM[3], "%d", num2);
            }
            strcpy(paramsSM[6], SMResults2);

            printf(" I am the child process %d ", getpid());
            printf(" and will be replaced with ’ splitterMerger ’\n");
            execvp("./splitterMerger", paramsSM);
            exit(1);
        }

        if (wait(&status) != pid1)
        { // check if child returns
            perror(" wait ");
            exit(1);
        }
        printf(" Child terminated with exit code %d\n", status >> 8);
    }
    else
    { //child 1
        if (sflag)
        {
            //have to think about it
        }
        else
        {
            paramsSM[2] = strcpy(paramsSM[2], argv[2]);
            sprintf(paramsSM[3], "%d", num1);
        }
        strcpy(paramsSM[6], SMResults1);
        printf(" I am the child process %d ", getpid());
        printf(" and will be replaced with ’ splitterMerger ’\n");
        execvp("./splitterMerger", paramsSM);
        exit(1);
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    char *datafile;
    int rangeBeg;
    char *pattern;
    int numOfrecords;
    int h;
    int sflag = 0;
    char *MyResults;
    if (argc < 7)
    {
        printf("filename, rangeBeg, numOfrecords, Pattern, height, Results \n");
        exit(1);
    }
    if (argc == 7)
    {
        // -s flag is used
        sflag = 1;
    }
    datafile = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(datafile, argv[1]);
    rangeBeg = atoi(argv[2]);
    numOfrecords = atoi(argv[3]);
    pattern = (char *)malloc(strlen(argv[4]) + 1);
    strcpy(pattern, argv[4]);
    h = atoi(argv[5]);
    MyResults = (char *)malloc(strlen(argv[6]) + 1);
    strcpy(MyResults, argv[6]);
    //
    if (h == 1)
    {
        spawnKids(argv, datafile, rangeBeg, pattern, numOfrecords, h, sflag, MyResults);
    }
    else
    {
        spawnSMs(argc, argv, datafile, rangeBeg, pattern, numOfrecords, h, sflag);
    }
}