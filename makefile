COMPILE_MACRO := gcc -g -ansi -pedantic -Wall

main: main.c list.o memory.o logging.o codesection.o datasection.o parser.o symboltable.o opcodemap.o
	$(COMPILE_MACRO) list.o memory.o logging.o codesection.o datasection.o opcodemap.o symboltable.o parser.o main.c -o main

parser.o: parser.c parser.h
	$(COMPILE_MACRO) -c parser.c
	
symboltable.o: symboltable.c symboltable.h
	$(COMPILE_MACRO) -c symboltable.c

opcodemap.o: opcodemap.c opcodemap.h	
	$(COMPILE_MACRO) -c opcodemap.c

datasection.o: datasection.c datasection.h
	$(COMPILE_MACRO) -c datasection.c

codesection.o: codesection.c codesection.h
	$(COMPILE_MACRO) -c codesection.c

logging.o: logging.c logging.h
	$(COMPILE_MACRO) -c logging.c

memory.o: memory.c memory.h
	$(COMPILE_MACRO) -c memory.c

list.o: list.c list.h
	$(COMPILE_MACRO) -c list.c

clean:
	# the -@ is so the make wont fail it threre were no files to delete
	-@rm *.o *~ main
