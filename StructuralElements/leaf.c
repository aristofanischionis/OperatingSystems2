#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../HeaderFiles/Records.h"
#include "../HeaderFiles/FileHandling.h"

#define MSGSIZE 65

char *fifo = "myfifo";

int main ( int argc , char * argv []) {
    int fd , i , nwrite ;
    char *datafile;
    int rangeBeg, rangeEnd;
    char *pattern;
    FILE *fpb;
    MyRecord rec;
    long lSize;
    int numOfrecords;
    // char msgbuf[ MSGSIZE +1];
    if ( argc != 4) { printf( "filename, rangeBeg, rangeEnd, Pattern \n" ); exit(1); }
    if ( ( fd = open( fifo , O_WRONLY | O_NONBLOCK )) < 0)
        { perror("fifo open error" ); exit(1); }

    datafile = (char *)malloc(strlen(argv[1]) * sizeof(char) + 1);
    strcpy(datafile, argv[1]);
    rangeBeg = atoi(argv[2]);
    rangeEnd = atoi(argv[3]);
    pattern = (char *)malloc(strlen(argv[4]) * sizeof(char) + 1);
    strcpy(pattern, argv[4]);

    // read records from file

    fpb = FileRead(datafile);
        // check number of records
    fseek (fpb , rangeBeg , rangeEnd);
    lSize = ftell (fpb);
    rewind (fpb);
    numOfrecords = (int)lSize/sizeof(rec);

    printf("Records found in file %d \n", numOfrecords);
    sleep(5);

    for (i=0; i<numOfrecords ; i++) {
            fread(&rec, sizeof(rec), 1, fpb);
            printf("%ld %s %s  %s %d %s %s %-9.2f\n", \
            rec.AM, rec.LastName, rec.FirstName, \
            rec.Street, rec.HouseID, rec.City, rec.postcode, \
            rec.salary);
            //  check if pattern exists in file

            if( strstr(rec.AM, pattern) ){
                // do stuff
            }
            else if(strstr(rec.LastName, pattern)) {

            }
            else if(strstr(rec.FirstName, pattern)){
                
            }
            else if(strstr(rec.Street, pattern)){
                
            }
            else if(strstr(rec.HouseID, pattern)){
                
            }
            else if(strstr(rec.City, pattern)){
                
            }
            else if(strstr(rec.postcode, pattern)){
                
            }
            else if(strstr(rec.salary, pattern)){
                
            }
        }

    

    fclose (fpb);
    // for (i = 1; i < argc ; i ++) {
    //     if ( strlen( argv[i] ) > MSGSIZE ) {
    //         printf( " Message with Prefix %.*s Too long - Ignored \n " ,10 , argv [ i ]) ;
    //         fflush( stdout );
    //         continue ;
    //     }
    //     strcpy( msgbuf , argv [ i ]) ;
    //     if (( nwrite = write(fd , msgbuf , MSGSIZE +1) ) == -1)
    //     { perror(" Error in Writing " ); exit (2) ; }
    // }
    exit(0);
}