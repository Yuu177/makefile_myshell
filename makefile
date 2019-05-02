makeshell: main.o commands.o 
	gcc main.o commands.o -o makeshell
main.o: main.c commands.h
	gcc -c main.c
commands.o: commands.c commands.h
	gcc -c commands.c
clean:
	rm *.o
	rm *.h.gch
