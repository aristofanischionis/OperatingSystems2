#include <stdio.h>
// #include <string.h>
#include <stdlib.h> 
// #include <errno.h> 

FILE* FileRead (char *in){
    FILE * input;
    input = fopen (in, "rb");
    if (input == NULL) {
        fprintf(stderr, "Can't open binary file %s\n", in);
        return(NULL);
    }
    return input;
}
