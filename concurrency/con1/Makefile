default: program

program.o: concurrency.c
	gcc -o mt19937ar.o -c mt19937ar.c

program: program.o
	gcc -g concurrency.c mt19937ar.o -o concurrency -lpthread
	rm -f mt19937ar.o
