EXE = ttytt
OBJECTS = ttytt.o
CFLAGS = -g -lncurses
parser : $(OBJECTS) 
	gcc -o $(EXE) $(CFLAGS) $(OBJECTS) 

$(OBJECTS) : 
.PHONY : clean
clean :
	rm $(EXE) $(OBJECTS)
