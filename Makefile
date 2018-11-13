#Got this from the official Website of Alexis Delis

OBJS 	= main.o API.o
SOURCE	= main.c API.c
HEADER  = HeaderFiles/API.h HeaderFiles/Records.h
OUT  	= myfind leaf splitterMerger
CC	= gcc
FLAGS   = -g -c -Wall
# -g option enables debugging mode 
# -c flag generates object code for separate files
# make the executables

all: $(OUT)

myfind : $(OBJS)
	$(CC) -g $(OBJS) -o myfind

leaf : leaf.o
	$(CC) -Wall -o leaf leaf.c

splitterMerger : splitterMerger.o
	$(CC) -Wall -o splitterMerger splitterMerger.c

# create/compile the individual files >>separately<< 

main.o: main.c
	$(CC) $(FLAGS) main.c

API.o: API.c
	$(CC) $(FLAGS) API.c

# clean house
clean:
	rm -f $(OBJS) $(OUT) leaf.o splitterMerger.o leaf splitterMerger

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)