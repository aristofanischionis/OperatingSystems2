#Got this from the official Website of Alexis Delis

OBJS 	= main.o BinaryTreeofProcesses.o API.o FileHandling.o InputManager.o Records.o
SOURCE	= main.c CodeFiles/BinaryTreeofProcesses.c CodeFiles/API.c CodeFiles/FileHandling.c CodeFiles/InputManager.c CodeFiles/Records.c 
HEADER  = HeaderFiles/BinaryTreeofProcesses.h HeaderFiles/API.h HeaderFiles/FileHandling.h HeaderFiles/InputManager.h HeaderFiles/Records.h
OUT  	= myfind
CC	= gcc
FLAGS   = -g -c -Wall
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

BinaryTreeofProcesses.o: CodeFiles/BinaryTreeofProcesses.c
	$(CC) $(FLAGS) CodeFiles/BinaryTreeofProcesses.c

API.o: CodeFiles/API.c
	$(CC) $(FLAGS) CodeFiles/API.c

FileHandling.o: CodeFiles/FileHandling.c
	$(CC) $(FLAGS) CodeFiles/FileHandling.c

InputManager.o: CodeFiles/InputManager.c
	$(CC) $(FLAGS) CodeFiles/InputManager.c

Records.o: CodeFiles/Records.c
	$(CC) $(FLAGS) CodeFiles/Records.c
# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)