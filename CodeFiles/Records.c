#include <stdio.h>
#include <unistd.h>
#include "../HeaderFiles/Records.h"
#include "../HeaderFiles/FileHandling.h"

int ReadAllRecords(char *datafile){
    FILE *fpb;
    MyRecord rec;
    long lSize;
    int numOfrecords, i;

    fpb = FileRead(datafile);
        // check number of records
    fseek (fpb , 0 , SEEK_END);
    lSize = ftell (fpb);
    rewind (fpb);
    numOfrecords = (int) lSize/sizeof(rec);

    printf("Records found in file %d \n", numOfrecords);
    sleep(5);

    for (i=0; i<numOfrecords ; i++) {
            fread(&rec, sizeof(rec), 1, fpb);
            printf("%ld %s %s  %s %d %s %s %-9.2f\n", \
            rec.AM, rec.LastName, rec.FirstName, \
            rec.Street, rec.HouseID, rec.City, rec.postcode, \
            rec.salary);
        }

    fclose (fpb);
    return 0;
}