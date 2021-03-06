CC= gcc
CFLAGS= -O3

all:
	$(CC) $(CFLAGS) worker.c -o worker.out -lpthread -lm
	$(CC) $(CFLAGS) main.c -o lockstep.out -lpthread -lm

clean:
	rm -f *.exe *.out *.o
