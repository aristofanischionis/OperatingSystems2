#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
// #include <poll.h>
#include "./HeaderFiles/Records.h"

void readWritefifos(int fd, int myfd)
{
    printf("I am %d and now i'm gonna read from my pipe\n", getpid());
    int nread = 0;
    MyRecord rec;
    MyRecord rec2;
    while ((nread = read(fd, &rec, sizeof(rec)) > 0))
    {
        if (rec.AM == -1)
        {
            // printf("THIS IS READWRITEFIFOSI just read end\n");
            MyRecord rec1;
            rec1.AM = -1;
            //
            strncpy(rec1.LastName, "rec1.LastName", 20);
            strncpy(rec1.FirstName, "rec1.FirstName", 20);
            strncpy(rec1.Street, "rec1.Street", 20);
            rec1.HouseID = 0;
            strncpy(rec1.City, "rec1.City", 20);
            strncpy(rec1.postcode, "rec1", 6);
            rec1.salary = 13;

            //
            if (write(myfd, &rec1, sizeof(rec1)) == -1)
            { // to specify that statistics are coming for the dad
                perror(" Error in Writing in pipe\n");
                exit(2);
            }
            break;
        }
        // printf("THIS IS WHAT I READ------->%ld %s %s  %s %d %s %s %-9.2f\n",
        //        rec.AM, rec.LastName, rec.FirstName,
        //        rec.Street, rec.HouseID, rec.City, rec.postcode,
        //        rec.salary);

        rec2.AM = rec.AM;
        strncpy(rec2.LastName, rec.LastName, 20);
        strncpy(rec2.FirstName, rec.FirstName, 20);
        strncpy(rec2.Street, rec.Street, 20);
        rec2.HouseID = rec.HouseID;
        strncpy(rec2.City, rec.City, 20);
        strncpy(rec2.postcode, rec.postcode, 6);
        rec2.salary = rec.salary;
        if (write(myfd, &rec2, sizeof(rec2)) == -1)
        {
            perror(" Error in Writing in pipe\n");
            exit(2);
        }
    }
    char stat[25];
    nread = read(fd, stat, sizeof(stat));
    printf("time for kid %d is %s\n", getpid(), stat);
    // write this time to my dad's pipe
    if (write(myfd, stat, sizeof(stat)) == -1)
    {
        perror(" Error in Writing in pipe\n");
        exit(2);
    }
    return;
}

void spawnKids(
    char *argv[],
    char *datafile,
    int rangeBeg,
    char *pattern,
    int numOfrecords,
    int h,
    int sflag,
    int myfd)
{
    char *params[7];
    int num1, num2;
    params[0] = (char *)malloc(5);
    strcpy(params[0], "./leaf");
    params[1] = (char *)malloc(strlen(datafile) + 1);
    strcpy(params[1], datafile);
    params[2] = (char *)malloc(12);
    params[3] = (char *)malloc(12);
    params[4] = (char *)malloc(strlen(pattern) + 1);
    strcpy(params[4], pattern);
    params[5] = (char *)malloc(30);
    params[6] = NULL;
    num1 = numOfrecords / 2;
    num2 = numOfrecords - num1;

    int fd1, fd2;
    // to handle the case of odd numbersof records
    //
    char KidResults1[30];
    char KidResults2[30];
    // giving name to each fifo
    sprintf(KidResults1, "%s%d", "KidRes1_", getpid());
    sprintf(KidResults2, "%s%d", "KidRes2_", getpid());
    //
    // making the fifos for my kids
    if (mkfifo(KidResults1, 0666) < 0)
    {
        perror(" Error creating the named pipe ");
        exit(1);
    }
    if (mkfifo(KidResults2, 0666) < 0)
    {
        perror(" Error creating the named pipe ");
        exit(1);
    }

    // placing the timer
    clock_t begin = clock();
    // forking the kids
    pid_t pid;
    for (int i = 0; i < 2; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            // do childern stuff
            if (i == 0)
            { // child 1
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
            else
            { // child 2
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
        }
        else if (pid)
        {
            // waitpid(pid, &status, 0);
            continue;
        }
        else
        {
            printf("Error in forking kids\n");
            exit(1);
        }
    }
    // parent
    printf(" I am the parent process % d\n", getpid());
    // while ((pid1 = waitpid(-1, &status, 0)) != -1)
    // {
    //     printf("Process %d terminated\n", pid1);
    // }
    if ((fd1 = open(KidResults1, O_RDONLY)) < 0)
    {
        perror("fifo open error");
        exit(1);
    }
    // while ((pid2 = waitpid(-1, &status, 0)) != -1)
    //     {
    //         printf("Process %d terminated\n", pid2);
    //     }

    if ((fd2 = open(KidResults2, O_RDONLY)) < 0)
    {
        perror("fifo open error");
        exit(1);
    }
    readWritefifos(fd2, myfd);
    readWritefifos(fd1, myfd);

    // end of timer
    clock_t end = clock();
    MyRecord rec;
    rec.AM = -1;
    //
    strncpy(rec.LastName, "rec.LastName", 20);
    strncpy(rec.FirstName, "rec.FirstName", 20);
    strncpy(rec.Street, "rec.Street", 20);
    rec.HouseID = 0;
    strncpy(rec.City, "rec.City", 20);
    strncpy(rec.postcode, "rec", 6);
    rec.salary = 13;

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
    // remove files
    if (remove(KidResults2) == 0)
            printf("Deleted successfully");
    else
        printf("Unable to delete the file");
    if (remove(KidResults1) == 0)
            printf("Deleted successfully");
    else
        printf("Unable to delete the file");
    //
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
    int sflag,
    int myfd)
{
    char *paramsSM[argc];
    int num1, num2;
    int fd1, fd2;
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
    paramsSM[6] = (char *)malloc(30); // namedpipe for results
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

    char SMResults1[30];
    char SMResults2[30];
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

    // placing the timer
    clock_t begin = clock();
    //forking the kids
    pid_t pid;
    for (int i = 0; i < 2; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            // do childern stuff
            if (i == 0)
            { // child 1
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
            else
            { // child 2
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
        }
        else if (pid)
        {
            // waitpid(pid, &status, 0);
            continue;
        }
        else
        {
            printf("Error in forking kids\n");
            exit(1);
        }
    }
    // parent
    printf(" I am the parent process % d\n", getpid());
    if ((fd2 = open(SMResults2, O_RDONLY)) == -1)
    {
        perror("fifo open error");
        exit(1);
    }
    if ((fd1 = open(SMResults1, O_RDONLY)) == -1)
    {
        perror("fifo open error");
        exit(1);
    }
    // read and write to my dad's pipe
    readWritefifos(fd2, myfd);
    readWritefifos(fd1, myfd);
    // end of timer
    clock_t end = clock();
    MyRecord rec;
    rec.AM = -1;
    //
    strncpy(rec.LastName, "rec.LastName", 20);
    strncpy(rec.FirstName, "rec.FirstName", 20);
    strncpy(rec.Street, "rec.Street", 20);
    rec.HouseID = 0;
    strncpy(rec.City, "rec.City", 20);
    strncpy(rec.postcode, "rec", 6);
    rec.salary = 13;

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
    // remove files
    if (remove(SMResults1) == 0)
            printf("Deleted successfully");
    else
        printf("Unable to delete the file");
    if (remove(SMResults2) == 0)
            printf("Deleted successfully");
    else
        printf("Unable to delete the file");
    //
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
    if (argc == 8)
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
    int myfd;
    if ((myfd = open(MyResults, O_WRONLY)) == -1)
    {
        perror("fifo open error");
        exit(1);
    }
    if (h == 1)
    {
        spawnKids(argv, datafile, rangeBeg, pattern, numOfrecords, h, sflag, myfd);
    }
    else
    {
        spawnSMs(argc, argv, datafile, rangeBeg, pattern, numOfrecords, h, sflag, myfd);
    }
}