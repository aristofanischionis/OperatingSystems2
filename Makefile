#Got this from the official Website of Alexis Delis

OBJS 	= main.o API.o Records.o
SOURCE	= main.c CodeFiles/API.c CodeFiles/Records.c 
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

leaf : StructuralElements/leaf.o
	$(CC) -g StructuralElements/leaf.o -o StructuralElements/leaf

splitterMerger : StructuralElements/splitterMerger.o
	$(CC) -g StructuralElements/splitterMerger.o -o StructuralElements/splitterMerger

# create/compile the individual files >>separately<< 
splitterMerger.o : StructuralElements/splitterMerger.c
	$(CC) $(FLAGS) StructuralElements/splitterMerger.c

leaf.o : StructuralElements/leaf.c
	$(CC) $(FLAGS) StructuralElements/leaf.c

main.o: main.c
	$(CC) $(FLAGS) main.c

API.o: CodeFiles/API.c
	$(CC) $(FLAGS) CodeFiles/API.c

Records.o: CodeFiles/Records.c
	$(CC) $(FLAGS) CodeFiles/Records.c
# clean house
clean:
	rm -f $(OBJS) $(OUT) StructuralElements/leaf.o StructuralElements/splitterMerger.o StructuralElements/leaf StructuralElements/splitterMerger

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)