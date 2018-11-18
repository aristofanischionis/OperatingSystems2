#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include "./HeaderFiles/Records.h"

int findSubstring(char *tobechecked, char *pattern){
    if(strstr(tobechecked, pattern)){
        // printf("My pid is %d and I found the record: %s\n", getpid(), tobechecked);
        return 1;
    }
    else return 0;
}

int main ( int argc , char * argv []) {
    int i, sum=0;
    char *datafile;
    int rangeBeg;
    char *pattern;
    FILE *fpb;
    int fd;
    MyRecord rec;
    MyRecord rec2 ;
    int numOfrecords;
    char tobechecked[sizeof(rec)+1];
    if ( argc != 7) { 
        printf( "filename, rangeBeg, numOfrecords, Pattern, results, parentPid\n" ); 
        exit(1); 
    }
    datafile = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(datafile, argv[1]);
    rangeBeg = atoi(argv[2]);
    numOfrecords = atoi(argv[3]);
    pattern = (char *)malloc(strlen(argv[4]) + 1);
    strcpy(pattern, argv[4]);
    // taking fifo name for results and statistics
    char *fifo = (char *)malloc(strlen(argv[5]) + 1);
    strcpy(fifo, argv[5]);
    pid_t parentPid;
    parentPid = atoi(argv[6]);
    // opening results fifo
    if ( ( fd = open( fifo , O_WRONLY )) == -1){ 
        perror("fifo open error" ); 
        exit(1); 
    }
    // read records from file
    fpb = fopen (datafile,"rb");
    if (fpb==NULL) {
      	printf("Cannot open binary file\n");
      	return 1;
   	}
    //place timer
    clock_t begin = clock();
    // place cursor in the rangeBeg record
    fseek (fpb , (long)rangeBeg*sizeof(rec) , SEEK_SET);
    for (i=0; i<numOfrecords ; i++) {
        fread(&rec, sizeof(rec), 1, fpb);
        // place the record in a string to check it for substrings
        int n = sprintf(tobechecked, "%ld %s %s %s %d %s %s %-9.2f\n", \
        rec.AM, rec.LastName, rec.FirstName, \
        rec.Street, rec.HouseID, rec.City, rec.postcode, \
        rec.salary);
        if( n == 0) {
            printf("snprintf read 0 chars\n");
            exit(1);
        }
        //  check if pattern exists in file
        if(findSubstring(tobechecked , pattern) ){
            // store this record to the pipe
            sum++;
            rec2.AM = rec.AM;
            strncpy(rec2.LastName,rec.LastName,20);
            strncpy(rec2.FirstName,rec.FirstName,20);
            strncpy(rec2.Street,rec.Street,20);
            rec2.HouseID = rec.HouseID;
            strncpy(rec2.City,rec.City,20);
            strncpy(rec2.postcode,rec.postcode,6);
            rec2.salary = rec.salary;
            if (write(fd, &rec2, sizeof(rec2)) == -1){ 
                perror(" Error at Writing in pipe\n" ); 
                exit(2) ;
            }
        }
    }
    fclose(fpb);
    printf("I am leaf %d and I found %d records in my range\n",getpid(), sum);
    clock_t end = clock();
    rec2.AM = -1;
    //
    if (write(fd, &rec2, sizeof(rec2)) == -1){  // to specify that statistics are coming
        perror(" Error at Writing in pipe\n" ); 
        exit (2) ;
    }
    char tobewritten[25];
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    // printf("KIDDO PID %d needed ----------->%f\n",getpid(), time_spent);
    sprintf(tobewritten, "Searcher %f", time_spent );
    if ((write(fd, tobewritten, sizeof(tobewritten)) ) == -1){ 
        perror(" Error at Writing in pipe\n" ); 
        exit (2) ;
    }
    // send end signal to root
    printf("\nKIDDO: sending SIGUSR2\n\n");
    kill(parentPid,SIGUSR2);

    exit(0);

}