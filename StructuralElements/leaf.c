#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#include "../HeaderFiles/Records.h"

char *fifo = "results";
char *fifo2 = "statistics";

int findSubstring(char *tobechecked, char *pattern){
    if(strstr(tobechecked, pattern)){
        printf("I found the record: %s\n", tobechecked);
        return 1;
    }
    else return 0;
}

int main ( int argc , char * argv []) {
    clock_t begin = clock();
    int i , nwrite ;
    char *datafile;
    int rangeBeg;
    char *pattern;
    FILE *fpb, *fd;
    MyRecord rec;
    MyRecord rec2 ;
    long lSize;
    int numOfrecords;
    char tobechecked[sizeof(rec)+1];
    if ( argc < 5) { 
        printf( "filename, rangeBeg, numOfrecords, Pattern \n" ); 
        exit(1); 
    }
    if ( ( fd = fopen( fifo , "wb" )) < 0){ 
        perror("fifo open error" ); 
        exit(1); 
    }

    datafile = (char *)malloc(strlen(argv[1]) * sizeof(char) + 1);
    strcpy(datafile, argv[1]);
    rangeBeg = atoi(argv[2]);
    numOfrecords = atoi(argv[3]);
    pattern = (char *)malloc(strlen(argv[4]) * sizeof(char) + 1);
    strcpy(pattern, argv[4]);

    // read records from file
    fpb = fopen (datafile,"rb");
    if (fpb==NULL) {
      	printf("Cannot open binary file\n");
      	return 1;
   	}
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
            printf("snprintf read 0 chars");
            exit(1);
        }
        //  check if pattern exists in file
        if(findSubstring(tobechecked , pattern) ){
            // store this record to the pipe
            rec2.AM = rec.AM;
            strncpy(rec2.LastName,rec.LastName,20);
            strncpy(rec2.FirstName,rec.FirstName,20);
            strncpy(rec2.Street,rec.Street,20);
            rec2.HouseID = rec.HouseID;
            strncpy(rec2.City,rec.City,20);
            strncpy(rec2.postcode,rec.postcode,6);
            rec2.salary = rec.salary;
            if (( nwrite = fwrite(&rec2, sizeof(rec2),1,fd) ) == -1){ 
                perror(" Error in Writing in pipe\n" ); 
                exit (2) ;
            }
        } 
    }

    fclose(fd);
    fclose(fpb);

// to read from pipe
    // if ( ( fd = fopen( fifo , "rb" )) < 0){ 
    //     perror("fifo open error" ); 
    //     exit(1); 
    // }
    // fseek (fd , 0 , SEEK_END);
    // lSize = ftell (fd);
    // rewind (fd);
    // numOfrecords = (int) lSize/sizeof(rec);
    // for (i=0; i<numOfrecords ; i++) {
    //     fread(&rec, sizeof(rec), 1, fd);
    //   	printf("%ld %s %s  %s %d %s %s %-9.2f\n", \
	// 	rec.AM, rec.LastName, rec.FirstName, \
	// 	rec.Street, rec.HouseID, rec.City, rec.postcode, \
	// 	rec.salary);
    // }
    
    //     fclose(fd);        
    FILE *fp;  
    if ( ( fp = fopen(fifo2,"w")) < 0){ 
        perror("fifo2 open error" ); 
        exit(1); 
    }
    
    clock_t end = clock();
    char tobewritten[50];
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("PID %d needed %f\n",getpid(), time_spent);
    sprintf(tobewritten, "PID %d needed %f\n", getpid(), time_spent );
    if (( nwrite = fwrite(tobewritten, sizeof(tobewritten),1,fp) ) == -1){ 
        perror(" Error in Writing in pipe\n" ); 
        exit (2) ;
    }
    fclose(fp);
    exit(0);

}