CC = gcc # compiler
CFLAGS = -Wall -g # compile flags
LIBS = -lrt # libs

all: shm-posix-producer shm-posix-consumer

shm-posix-producer: shm-posix-producer.o
	$(CC) -o shm-posix-producer shm-posix-producer.o $(LIBS)

shm-posix-consumer: shm-posix-consumer.o
	$(CC) -o shm-posix-consumer shm-posix-consumer.o $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -f shm-posix-producer shm-posix-consumer *.o *~

