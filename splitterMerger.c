#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "./HeaderFiles/Records.h"

void writeFakeRecord(int myfd)
{
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
}

void writeStat(int fd, int myfd){
    char stat[25];
    int nread = 0;
    nread = read(fd, stat, sizeof(stat));
    // printf("time for kid %d is %s\n", getpid(), stat);
    // write this time to my dad's pipe
    if (write(myfd, stat, sizeof(stat)) == -1)
    {
        perror(" Error in Writing in pipe\n");
        exit(2);
    }
}

void readWritefifos(int fd, int myfd)
{
    // printf("I am %d and now i'm gonna read from my pipe\n", getpid());
    int nread = 0;
    MyRecord rec;
    MyRecord rec2;
    while ((nread = read(fd, &rec, sizeof(rec)) > 0))
    {
        if (rec.AM == -1)
        {
            writeFakeRecord(myfd);
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
    writeStat(fd, myfd);
    return;
}

void readWriteSMfifos(int fd, int myfd){
    // printf("I am SM %d and now i'm gonna read from my pipe\n", getpid());
    int nread = 0;
    MyRecord rec;
    MyRecord rec2;
    while ((nread = read(fd, &rec, sizeof(rec)) > 0))
        {

            if (rec.AM == -1)
            {
            // printf("THIS IS readWriteSMfifos just read end\n");
            writeFakeRecord(myfd);
            writeStat(fd, myfd);
            continue;
            }
            // printf("HELLOOOO ITS SM ->>>>>%ld %s %s  %s %d %s %s %-9.2f\n",
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
}

int sumN(int h){
    int res = 1;
    int adder = 1;
    int h = 5;
    while(h){
        adder = 2 * adder;
        res = res + adder;
        h--;
    }
    return res;
}
int calculateSkew(int RecordsSflag, int h, int i){
    return (int)((RecordsSflag * i) / sumN(h));
}

int calculateSkewRangeBeg(int RecordsSflag, int h, int i){
// this function will calclate how many records 
//have already been read so that I know my range beg for skew case

}
void spawnKids(
    char *argv[],
    char *datafile,
    int rangeBeg,
    char *pattern,
    int numOfrecords,
    int h,
    int sflag,
    int myfd,
    char *parentPid,
    int RecordsSflag)
{
    char *params[8];
    int num1, num2, num3;
    int fd1, fd2;
    char KidResults1[30];
    char KidResults2[30];
    params[0] = (char *)malloc(5);
    params[1] = (char *)malloc(strlen(datafile) + 1);
    params[2] = (char *)malloc(12);
    params[3] = (char *)malloc(12);
    params[4] = (char *)malloc(strlen(pattern) + 1);
    params[5] = (char *)malloc(30);
    params[6] = (char *)malloc(10);
    strcpy(params[0], "./leaf");
    strcpy(params[1], datafile);
    //
    //
    strcpy(params[4], pattern);
    //
    strcpy(params[6], parentPid);
    params[7] = NULL;
    num1 = numOfrecords / 2;
    num2 = numOfrecords - num1;

    if(sflag){
        num1 = calculateSkew(RecordsSflag, h, rangeBeg);
        num2 = calculateSkew(RecordsSflag, h, numOfrecords);
        num3 = calculateSkewRangeBeg(RecordsSflag, h, rangeBeg);
    }
    
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
    struct timeval t0,t1;
    gettimeofday(&t0, NULL);
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
                    strcpy(params[2], argv[2]);
                }
                sprintf(params[3], "%d", num1);
                strcpy(params[5], KidResults1);
                // printf(" I am the child process %d ", getpid());
                // printf(" and will be replaced with ’ leaf ’\n");
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
                }
                sprintf(params[3], "%d", num2);
                strcpy(params[5], KidResults2);
                // printf(" I am the child process %d ", getpid());
                // printf(" and will be replaced with ’ leaf ’\n");
                execvp("./leaf", params);
                exit(1);
            }
        }
        else if (pid)
        {
            // pid_t wpid;
            // int status =0;
            // while((wpid = wait(&status)) > 0);
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
    // printf(" I am the parent process % d\n", getpid());
    if ((fd1 = open(KidResults1, O_RDONLY)) < 0)
    {
        perror("fifo open error");
        exit(1);
    }
    if ((fd2 = open(KidResults2, O_RDONLY)) < 0)
    {
        perror("fifo open error");
        exit(1);
    }
    readWritefifos(fd2, myfd);
    readWritefifos(fd1, myfd);

    // end of timer
    gettimeofday(&t1, NULL);
    double time_spent = (double) (t1.tv_usec - t0.tv_usec) / 1000000 + (double) (t1.tv_sec - t0.tv_sec);
    writeFakeRecord(myfd);
    
    char tobewritten[25];
    // printf("PID %d needed %f\n", getpid(), time_spent);
    sprintf(tobewritten, "SM %f", time_spent);
    if ((write(myfd, tobewritten, sizeof(tobewritten))) == -1)
    {
        perror(" Error at Writing in pipe\n");
        exit(2);
    }
    // remove files
    remove(KidResults2);
    remove(KidResults1);
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
    int myfd,
    char *parentPid,
    int RecordsSflag)
{
    int num = 9;
    if(sflag) num = 11;
    char *paramsSM[num];
    int num1, num2;
    int fd1, fd2;
    char SMResults1[30];
    char SMResults2[30];
    paramsSM[0] = (char *)malloc(5);
    paramsSM[1] = (char *)malloc(strlen(datafile) + 1);
    paramsSM[2] = (char *)malloc(12);
    paramsSM[3] = (char *)malloc(12);
    paramsSM[4] = (char *)malloc(strlen(pattern) + 1);
    paramsSM[5] = (char *)malloc(12);
    paramsSM[6] = (char *)malloc(30); // namedpipe for results
    paramsSM[7] = (char *)malloc(10);
    strcpy(paramsSM[0], "./splitterMerger");
    strcpy(paramsSM[1], datafile);
    // assigned in each kid
    // assigned in each kid
    strcpy(paramsSM[4], pattern);
    sprintf(paramsSM[5], "%d", h - 1);
    // assigned in each kid
    strcpy(paramsSM[7], parentPid);
    if (sflag)
    {
        paramsSM[8] = (char *)malloc(3);
        paramsSM[9] = (char *)malloc(12);
        strcpy(paramsSM[8], "-s");
        sprintf(paramsSM[9], "%d", RecordsSflag);
        paramsSM[10] = NULL;
    }
    else
        paramsSM[8] = NULL;

    // to handle the case of odd numbersof records
    num1 = numOfrecords / 2;
    num2 = numOfrecords - num1;
    //we do the same even if we have -s flag
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
    struct timeval t0,t1;
    gettimeofday(&t0, NULL);
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

                strcpy(paramsSM[2], argv[2]);
                sprintf(paramsSM[3], "%d", num1);
                strcpy(paramsSM[6], SMResults1);
                // printf(" I am the child process %d ", getpid());
                // printf(" and will be replaced with ’ splitterMerger ’\n");
                execvp("./splitterMerger", paramsSM);
                exit(1);
            }
            else
            { // child 2

                sprintf(paramsSM[2], "%d", rangeBeg + num1);
                sprintf(paramsSM[3], "%d", num2);
                strcpy(paramsSM[6], SMResults2);

                // printf(" I am the child process %d ", getpid());
                // printf(" and will be replaced with ’ splitterMerger ’\n");
                execvp("./splitterMerger", paramsSM);
                exit(1);
            }
        }
        else if (pid)
        {
            // pid_t wpid;
            // int status =0;
            // while((wpid = wait(&status)) > 0);
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
    // printf(" I am the parent process % d\n", getpid());
    if ((fd1 = open(SMResults1, O_RDONLY)) == -1)
    {
        perror("fifo open error");
        exit(1);
    }
    if ((fd2 = open(SMResults2, O_RDONLY)) == -1)
    {
        perror("fifo open error");
        exit(1);
    }
    // read and write to my dad's pipe
    readWriteSMfifos(fd2, myfd);
    readWriteSMfifos(fd1, myfd);
    // end of timer
    gettimeofday(&t1, NULL);
    double time_spent = (double) (t1.tv_usec - t0.tv_usec) / 1000000 + (double) (t1.tv_sec - t0.tv_sec);

    writeFakeRecord(myfd);

    char tobewritten[25];
    // printf("PID %d needed %f\n", getpid(), time_spent);
    sprintf(tobewritten, "SM %f", time_spent);
    if ((write(myfd, tobewritten, sizeof(tobewritten))) == -1)
    {
        perror(" Error at Writing in pipe\n");
        exit(2);
    }
    // remove files
    remove(SMResults1);
    remove(SMResults2);
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
    char *parentPid;
    int RecordsSflag= 0;
    if (argc < 8)
    {
        printf("filename, rangeBeg, numOfrecords, Pattern, height, Results, parentPid, -s, RecordsSflag \n");
        exit(1);
    }
    if (argc == 11)
    {
        // -s flag is used
        sflag = 1;
        RecordsSflag = atoi(argv[9]);
    }
    datafile = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(datafile, argv[1]);
    
    rangeBeg = atoi(argv[2]); // if given sflag this is 1
    numOfrecords = atoi(argv[3]); // if given sflag this is s^h

    pattern = (char *)malloc(strlen(argv[4]) + 1);
    strcpy(pattern, argv[4]);
    h = atoi(argv[5]);
    MyResults = (char *)malloc(strlen(argv[6]) + 1);
    strcpy(MyResults, argv[6]);
    parentPid = (char *)malloc(10);
    strcpy(parentPid, argv[7]);
    //
    int myfd;
    if ((myfd = open(MyResults, O_WRONLY)) == -1)
    {
        perror("fifo open error");
        exit(1);
    }
    if (h == 1)
    {
        spawnKids(argv, datafile, rangeBeg, pattern, numOfrecords, h, sflag, myfd, parentPid, RecordsSflag);
    }
    else
    {
        spawnSMs(argc, argv, datafile, rangeBeg, pattern, numOfrecords, h, sflag, myfd, parentPid, RecordsSflag);
    }
}