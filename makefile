gomoku:gomoku.o check.o board.o ai.o
	gcc gomoku.o check.o board.o ai.o -o gomoku
gomoku.o:
	gcc -c gomoku.c 
check.o:
	gcc -c check.c 
board.o:
	gcc -c board.c 
ai.o:
	gcc -c ai.c
clean:
	rm -f *.o gomoku
	