COMPILE_MACRO := gcc -g -ansi -pedantic -Wall

main: main.c parser.o symboltable.o
	$(COMPILE_MACRO) symboltable.o parser.o main.c -o main

parser.o: parser.c parser.h
	$(COMPILE_MACRO) -c parser.c
	

symboltable.o: symboltable.c symboltable.h
	$(COMPILE_MACRO) -c symboltable.c
clean:
	rm *.o *~ main
 
