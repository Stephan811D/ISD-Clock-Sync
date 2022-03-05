all: clock_sync
//CPP=arm-none-eabi-g++
CC=gcc

clock_sync.o: clock_sync.c clock_sync.h
	$(CC) -c -o clock_sync.o clock_sync.c

clock_sync_test.o: clock_sync_test.c clock_sync.h
	$(CC) -c -o clock_sync_test.o clock_sync_test.c

clock_sync: clock_sync.o clock_sync_test.o
	$(CC) -o clock_sync clock_sync.o clock_sync_test.o

clean:
	rm -rf *.o clock_sync
