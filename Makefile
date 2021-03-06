CC= gcc
CFLAGS= -O3 -std=gnu99

all:
	$(CC) $(CFLAGS) worker.c -o worker.out -lpthread -lm
	$(CC) $(CFLAGS) worker2.c -o worker2.out -lpthread -lm
	$(CC) $(CFLAGS) main.c -o lockstep.out -lpthread -lm

clean:
	rm -f *.exe *.out *.o
