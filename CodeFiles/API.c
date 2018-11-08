#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "../HeaderFiles/BinaryTreeofProcesses.h"
#include "../HeaderFiles/Records.h"
#include "../HeaderFiles/InputManager.h"
#include "../HeaderFiles/FileHandling.h"

#define MSGSIZE 65

char *fifo = "myfifo";

int InputDirector(int argc, char *argv[]){
    // Decode prompt ./myfind –h Height –d Datafile -p Pattern -s
    int i=1;
    int h=1;
    int sflag = 0;
    char *datafile;
    char *pattern;
    // datafile = (char *)malloc(40);
    // pattern = (char *)malloc(40);
    // strcpy(datafile, "");
    // strcpy(pattern, "");
    while( i<argc ){
        if( strcmp(argv[i],"-h") == 0 ){
            if( i < argc -1 ){
                if(strcmp(argv[i+1], "-d") == 0){
                    printf("Height not given after -h flag\n");
                    break; // giving the default value 1 to height.
                }
                printf("Height is: %s\n", argv[i+1]);
                h = atoi(argv[i+1]);
                if(h > 5 || h < 1){
                    printf("Acceptable heights are from 1-5, you gave %d, I will put the default h=1\n",h);
                    h=1;
                }
                break;
            }
            else{
                printf("Height not given after -h flag\n");
                break;
            }
        }
        i++;
    }
    i=1;
    while( i<argc ){
        if( strcmp(argv[i],"-d") == 0 ){
            if( i < argc -1 ){
                if(strcmp(argv[i+1], "-p") == 0){
                    printf("Datafile not given after -d flag, program will terminate now\n");
                    return 1;
                }
                printf("Datafile is: %s\n", argv[i+1]);
                datafile = (char *)malloc(strlen(argv[i+1]) * sizeof(char) + 1);
                strcpy(datafile, argv[i+1]);
                break;
            }
            else{
                printf("Datafile not given after -d flag, program will terminate now\n");
                return 1;
            }
        }
        i++;
    }
    i=1;
    while( i<argc ){
        if( strcmp(argv[i],"-p") == 0 ){
            if( i < argc -1 ){
                if(strcmp(argv[i+1], "-s") == 0){
                    printf("Pattern not given after -p flag, program will terminate now\n");
                    return 1;
                }
                printf("Pattern is: %s\n", argv[i+1]);
                pattern = (char *)malloc(strlen(argv[i+1]) * sizeof(char) + 1);
                strcpy(pattern, argv[i+1]);
                break;
            }
            else{
                printf("Pattern not given after -p flag, program will terminate now\n");
                return 1;
            }
        }
        i++;
    }
    i=1;
    while( i<argc ){
        if( strcmp(argv[i],"-s") == 0 ){
            printf("s flag is now enabled\n");
            sflag = 1;
        }
        i++;
    }
    // if(strcmp(datafile, "") != 0){
        // ReadAllRecords(datafile);
    // }
    // else printf("datafile Name not given\n");
    // if(strcmp(pattern, "") != 0){
    //     // InputManager(MyHash_Table, input);
    // }
    // else printf("pattern not given\n");
    // // the father process should make fork and call exec to execute the program for the splitter/merger and for the leaf nodes

    // makeProcesses(h);

    
    return 0;
}


int makeNamedPipe(){
    int fd , i , nwrite ;
    char msgbuf[ MSGSIZE +1];
    // make fifo
    if ( mkfifo( fifo , 0666) == -1 ){
        if ( errno != EEXIST ) {
            perror( " receiver : mkfifo " );
            exit(6) ;
        }
    }
    // open fifo
    if ( ( fd = open( fifo , O_RDWR )) < 0) {
        perror(" fifo open problem ");
        exit(3) ;
    }
    // ready to read from fifo
    for (;;) {
        if ( read(fd , msgbuf , MSGSIZE +1) < 0) {
            perror( " problem in reading ") ;
            exit(5) ;
        }
        printf(" \nMessage Received : %s\n" , msgbuf );
        fflush( stdout );
    }
}