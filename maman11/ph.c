#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <inttypes.h>

#include "binsem.h"
#include "ut.h"



#define LEFT  (i+N-1)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY   1
#define EATING   2

int N;

volatile int *phil_state;
sem_t *s;
sem_t mutex;
int *tid;

uint64_t get_wall_time() {
 struct timeval time;

 gettimeofday(&time, NULL); 
 uint64_t millis = (time.tv_sec * (uint64_t)1000) + (time.tv_usec / 1000);

  
 return millis;
}

void think(int p) {
  int i, factor;
  volatile int j;

  printf("Philosopher (%d) - time %" PRId64 " - is thinking\n",p, get_wall_time()); fflush (stdout);

  factor = 1 + random()%5;

  for (i = 0; i < 100000000*factor; i++){
    j += (int) i*i;
  }
            
  printf("Philosopher (%d) - time %" PRId64 " - is hungry\n", p, get_wall_time()); fflush (stdout);
}

void eat(int p){
  int i, factor;
  volatile int j;

   printf("Philosopher (%d) - time %" PRId64 " - is eating\n",p, get_wall_time()); fflush (stdout);

   factor = 1 + random()%5;
   for (i = 0; i < 100000000*factor; i++){
      j += (int) i*i;
   }
   //printf("Philosopher (%d) - time %" PRId64 " - is thinking\n",p, get_wall_time()); fflush (stdout);
}

void test(int i){
  if (phil_state[i] == HUNGRY &&
      phil_state[LEFT] != EATING &&
      phil_state[RIGHT] != EATING){
    phil_state[i] = EATING;

    binsem_up(&(s[i]));
  }
}
      
void take_forks(int i){
  binsem_down(&mutex);

  phil_state[i] = HUNGRY;

  test(i);

  binsem_up(&mutex);

  binsem_down(&(s[i]));
}
  

void put_forks(int i){
  binsem_down(&mutex);
  
  phil_state[i] = THINKING;

  test(LEFT);
  test(RIGHT);

  binsem_up(&mutex);
}

void int_handler(int signo) {
  long int duration;
  int i;

  for (i = 0; i < N; i++) {
    duration = ut_get_vtime(tid[i]);
    printf("Philosopher (%d) used the CPU %ld.%ld sec.\n",
	   i,duration/1000,duration%1000);
  }
  exit(0);
}

void philosopher(int i){
  while (1){
    think(i);
    take_forks(i);
    eat(i);
    put_forks(i);
  }
}

int main(int argc, char *argv[])
{
  int c;
  if (argc != 2){
    printf("Usage: %s N\n", argv[0]);
    exit(1);
  }
  
  N = atoi(argv[1]);

  if (N < 2){
    printf("Usage: %s N (N >=2)\n", argv[0]);
    exit(1);
  }
  
  ut_init(N);
  s = (sem_t *)malloc (N * sizeof(sem_t));
  phil_state = (int *) malloc (N * sizeof(int));
  tid = (int *) malloc (N * sizeof(int));
  
  for (c = 0; c < N ; c++){
    phil_state[c] = THINKING;
    binsem_init(&(s[c]), 0);
  }

  for (c = 0; c < N ; c++){
    tid[c] = ut_spawn_thread(philosopher,c);
    printf("Spawned thread #%d\n", tid[c]);
  }

  binsem_init(&mutex, 1);

  signal(SIGINT,int_handler);
  ut_start();
 
  return 0; // avoid warnings
  
}
