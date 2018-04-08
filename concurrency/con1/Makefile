default: program

program.o: con.c
	gcc -o con.o -c con.c -lpthread

program: program.o
	gcc con.o -o con -lpthread
	rm -f con.o
