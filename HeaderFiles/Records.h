#define SIZEofBUFF 20
#define SSizeofBUFF 6
#define READ 0
#define WRITE 1

typedef struct{
	long  	AM;
	char 	FirstName[SIZEofBUFF];
	char 	LastName[SIZEofBUFF];
	char	Street[SIZEofBUFF];
	int 	HouseID;
	char	City[SIZEofBUFF];
	char	postcode[SSizeofBUFF];
	float  	salary;
} MyRecord;

int ReadAllRecords(char *datafile);