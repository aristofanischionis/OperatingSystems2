#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int makeProcesses(int h){
    pid_t childpid;
    int i, j;
    childpid=-1;
    for (i = 0; i < h; i++) {
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