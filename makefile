CC=g++
CFLAGS=-I.
main: fullsampling.o
	$(CC) -o main fullsampling.o -I.
clean:
	rm -f *.o