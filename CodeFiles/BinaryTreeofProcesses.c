#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int makeProcesses(int argc, char *argv[]){
    pid_t childpid;
    int i, n, j;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s n\n", argv[0]); return 1;
    }
    n = atoi(argv[1]);
    childpid=-1;
    for (i = 0; i < n; i++) {
        for(j=0; j<2; j++) {
            childpid=fork();
            if (childpid <= 0) break;
        }
        if (childpid > 0) break;
    }

    while(wait(NULL) > 0) ; /* wait for all of your children */

    fprintf(stderr, "i:%d process ID:%ld parent ID:%ld child ID:%ld\n",i, (long)getpid(), (long)getppid(), (long)childpid);
    return 0;
}