COMPILE_MACRO := gcc -g -ansi -pedantic -Wall

main: main.c parser.o
	$(COMPILE_MACRO) parser.o main.c -o main

parser.o: parser.c parser.h
	$(COMPILE_MACRO) -c parser.c
clean:
	rm *.o *~ main
 
