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

int main ( int argc , char * argv []) {
    int num1, num2;
    char *datafile;
    int rangeBeg;
    char *pattern;
    int numOfrecords;
    int h;
    int sflag = 0;
    pid_t pid1, pid2;
    int  status;
    char *params[7];
    if( argc < 6) { 
        printf( "filename, rangeBeg, numOfrecords, Pattern, height \n" ); 
        exit(1); 
    }
    if(argc == 7){
        // -s flag is used
        sflag = 1;
    }
    datafile = (char *)malloc(strlen(argv[1])+ 1);
    strcpy(datafile, argv[1]);
    rangeBeg = atoi(argv[2]);
    numOfrecords = atoi(argv[3]);
    pattern = (char *)malloc(strlen(argv[4])+ 1);
    strcpy(pattern, argv[4]);
    h = atoi(argv[5]);

    if(h == 1){
        
        params[0]=(char *)malloc(5); strcpy(params[0], "./leaf");
        params[1]=(char *)malloc(strlen(datafile)+1); strcpy(params[1], datafile);
        params[2] = (char *)malloc(12);
        params[3] = (char *)malloc(12);
        params[4]=(char *)malloc(strlen(pattern)+1); strcpy(params[4], pattern);
        params[5] = (char *)malloc(20);
        params[6] = NULL;
        num1 = numOfrecords / 2;
        num2 = numOfrecords - num1;
        // to handle the case of odd numbersof records
        char KidResults1[20];
        char KidResults2[20];
        // giving name to each fifo
        sprintf(KidResults1, "%s%d","KidResults1_" ,getpid());
        sprintf(KidResults2, "%s%d","KidResults2_" ,getpid());
        //
        // making the fifos for my kids
        if( mkfifo(KidResults1, 0666) == -1){ perror( " Error creating the named pipe " ); exit(1); }
        if( mkfifo(KidResults2, 0666) == -1){ perror( " Error creating the named pipe " ); exit(1); }
        // forking the kids
        if ( ( pid1 = fork() ) == -1) { perror(" fork "); exit(1) ; }
        if ( pid1 !=0 ) { // parent
            printf (" I am the parent process % d\n" , getpid() ) ;
            if ( ( pid2 = fork() ) == -1) { perror(" fork "); exit(1) ; }
            if ( pid2 !=0 ) { // parent
                printf(" I am the parent process % d\n" , getpid() ) ;
                // if ( wait(&status) != pid2 ){ // check if child returns
                //     perror( " wait "); exit(1) ; }
                // printf (" Child terminated with exit code %d\n" , status >> 8) ;
                // reading from fifos 2
                printf("I am %s and now i'm gonna read from my pipe\n", KidResults2);
                int fd, nread = 0;
                MyRecord rec;
                if ( ( fd = open( KidResults2 ,  O_RDONLY  )) == -1){ 
                    perror("fifo open error" ); 
                    exit(1); 
                }
                
                while((nread = read(fd, &rec, sizeof(rec)) > 0)){
                    if(rec.AM == -1){
                        printf("I just read end");
                        break;
                    }

                    printf("%ld %s %s  %s %d %s %s %-9.2f\n", \
                        rec.AM, rec.LastName, rec.FirstName, \
                        rec.Street, rec.HouseID, rec.City, rec.postcode, \
                        rec.salary);

                }
                char stat2[12];
                nread = read(fd, stat2, sizeof(stat2));
                printf("time for kid 2 is %s\n", stat2);
                //
            //
            }
            else { //child 2
                if(sflag){
                    //have to think about it
                }
                else{
                    sprintf(params[2], "%d", rangeBeg + num1);
                    sprintf(params[3], "%d", num2);
                }
                strcpy(params[5], KidResults2);
                printf (" I am the child process %d " , getpid() );
                printf (" and will be replaced with ’ leaf ’\n" );
                execvp ("./leaf" , params );
                exit (1) ;
            }

            // if( wait(&status) != pid1 ){ // check if child returns
            //     perror( " wait "); exit (1) ; }
            // printf(" Child terminated with exit code %d\n" , status >> 8) ;
            // reading from fifos 1
            printf("I am %s and now i'm gonna read from my pipe\n", KidResults1);
            int fd, nread = 0;
            MyRecord rec;

            if ( ( fd = open( KidResults1 ,  O_RDONLY  )) == -1){ 
                perror("fifo open error" ); 
                exit(1); 
            }
            while((nread = read(fd, &rec, sizeof(rec)) > 0)){
                    if(rec.AM == -1){
                        printf("I just read end");
                        break;
                    }

                    printf("%ld %s %s  %s %d %s %s %-9.2f\n", \
                        rec.AM, rec.LastName, rec.FirstName, \
                        rec.Street, rec.HouseID, rec.City, rec.postcode, \
                        rec.salary);

                }
            char stat1[12];
                nread = read(fd, stat1, sizeof(stat1));
                printf("time for kid 1 is %s\n", stat1);

        }
        else { //child 1
            if(sflag){
                //have to think about it
            }
            else{
                params[2] = strcpy(params[2], argv[2]);              
                sprintf(params[3], "%d", num1);

            }
            strcpy(params[5], KidResults1);
            printf (" I am the child process %d " , getpid() );
            printf (" and will be replaced with ’ leaf ’\n" );
            execvp ("./leaf" , params );
            exit (1) ;
        }
        
        exit(0);
    }
    else {
        char *paramsSM[argc];
        
        paramsSM[0] = (char *)malloc(5); strcpy(paramsSM[0], "./splitterMerger");
        paramsSM[1] = (char *)malloc(strlen(datafile)+1); strcpy(paramsSM[1], datafile);
        paramsSM[2] = (char *)malloc(12);
        paramsSM[3] = (char *)malloc(12);
        paramsSM[4] = (char *)malloc(strlen(pattern)+1); strcpy(paramsSM[4], pattern);
        paramsSM[5] = (char *)malloc(12); sprintf(paramsSM[5], "%d", h - 1);
        if(sflag){
            paramsSM[6] = (char *)malloc(3); strcpy(paramsSM[6], "-s");
            paramsSM[7] = NULL;
        }
        else paramsSM[6] = NULL;
        
        num1 = numOfrecords / 2;
        num2 = numOfrecords - num1;
        // to handle the case of odd numbersof records
        if ( ( pid1 = fork() ) == -1) { perror(" fork "); exit(1) ; }
        if ( pid1 !=0 ) { // parent
            printf (" I am the parent process % d\n" , getpid() ) ;

            if ( ( pid2 = fork() ) == -1) { perror(" fork "); exit(1) ; }
            if ( pid2 !=0 ) { // parent
                printf(" I am the parent process % d\n" , getpid() ) ;
                if ( wait(&status) != pid2 ){ // check if child returns
                    perror( " wait "); exit(1) ; }
                printf (" Child terminated with exit code %d\n" , status >> 8) ;
            }
            else { //child 2
                if(sflag){
                    //have to think about it
                }
                else{
                    sprintf(paramsSM[2], "%d", rangeBeg + num1);
                    sprintf(paramsSM[3], "%d", num2);
                }

                printf (" I am the child process %d " , getpid() );
                printf (" and will be replaced with ’ splitterMerger ’\n" );
                execvp ("./splitterMerger" , paramsSM );
                exit (1) ;
            }

            if( wait(&status) != pid1 ){ // check if child returns
                perror( " wait "); exit (1) ; }
            printf(" Child terminated with exit code %d\n" , status >> 8) ;
        }
        else { //child 1
            if(sflag){
                //have to think about it
            }
            else{
                paramsSM[2] = strcpy(paramsSM[2], argv[2]);              
                sprintf(paramsSM[3], "%d", num1);
            }

            printf (" I am the child process %d " , getpid() );
            printf (" and will be replaced with ’ splitterMerger ’\n" );
            execvp ("./splitterMerger" , paramsSM );
            exit (1) ;
        }
        
        exit(0);

    }
}