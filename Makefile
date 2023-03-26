EXE = rentt
OBJECTS = rentt.o
CFLAGS = -g -lncurses
parser : $(OBJECTS) 
	gcc -o $(EXE) $(CFLAGS) $(OBJECTS) 

$(OBJECTS) : 
.PHONY : clean
clean :
	rm $(EXE) $(OBJECTS)
