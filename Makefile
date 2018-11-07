#Got this from the official Website of Alexis Delis

OBJS 	= main.o 
SOURCE	= main.c CodeFiles/BinaryTreeofProcesses.c 
HEADER  = HeaderFiles/BinaryTreeofProcesses.h
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

# API.o: CodeFiles/API.c
# 	$(CC) $(FLAGS) CodeFiles/API.c

# Repository.o: CodeFiles/Repository.c
# 	$(CC) $(FLAGS) CodeFiles/Repository.c

# HashTable.o: Graph/HashTable.c
# 	$(CC) $(FLAGS) Graph/HashTable.c

# Node.o: Graph/Node.c
# 	$(CC) $(FLAGS) Graph/Node.c

# Prime.o: Graph/Prime.c
# 	$(CC) $(FLAGS) Graph/Prime.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)