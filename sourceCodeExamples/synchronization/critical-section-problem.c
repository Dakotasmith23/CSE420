#define _GNU_SOURCE

#include <stdio.h>
#include "mythreads.h"
#include <stdlib.h>
#include <pthread.h>

int max;
volatile int balance = 0; // shared global variable

void * mythread(void *arg) {
  char *letter = arg;
  int i; // stack (private per thread) 
  printf("%s: begin\n", letter);
  for (i = 0; i < max; i++) {
    balance++; // shared: only one
  }
  printf("%s: done\n", letter);
  return NULL;
}
                                                                   
int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: critical-section-problem <loopcount>\n");
    exit(1);
  }
  max = atoi(argv[1]);

  pthread_t p1, p2;
  printf("main: begin\n [balance = %d]\n", balance);

  Pthread_create(&p1, NULL, mythread, "A"); 
  Pthread_create(&p2, NULL, mythread, "B");

  // join waits for the threads to finish
  Pthread_join(p1, NULL); 
  Pthread_join(p2, NULL); 

  printf("main: done\n\t[balance: %d]\n\t[should:  %d]\n", balance, max*2);
  return 0;
}

