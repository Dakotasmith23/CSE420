CC = gcc# compiler
CFLAGS = -Wall -g # compile flags
LIBS = -lpthread -lrt# libs

all: critical-section-problem solution-mutex lock-incorrect-solution  lock-correct-solution sem-pc file_locking_problem file_locking_solution

critical-section-problem: critical-section-problem.o
	$(CC) -o critical-section-problem critical-section-problem.o $(LIBS)

solution-mutex: solution-mutex.o
	$(CC) -o solution-mutex solution-mutex.o $(LIBS)

lock-incorrect-solution: lock-incorrect-solution.o
	$(CC) -o lock-incorrect-solution lock-incorrect-solution.o $(LIBS)

lock-correct-solution: lock-correct-solution.o
	$(CC) -o lock-correct-solution lock-correct-solution.o $(LIBS)

sem-pc: sem-pc.o
	$(CC) -o sem-pc sem-pc.o $(LIBS)

file_locking_problem: file_locking_problem.o
	$(CC) -o file_locking_problem file_locking_problem.o $(LIBS)

file_locking_solution: file_locking_solution.o
	$(CC) -o file_locking_solution file_locking_solution.o $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c

clean:
	rm -f critical-section-problem solution-mutex lock-incorrect-solution  lock-correct-solution sem-pc file_locking_problem file_locking_solution *.o *~ *.txt

