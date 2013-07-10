main: main.c	
	gcc -ansi -pedantic -Wall main.c -o main
 
clean:
	rm *.o *~ main
 
