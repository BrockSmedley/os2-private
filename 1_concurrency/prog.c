#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#include "./mt/mt19937ar.h"
#include "./stack.h"

#define BUFFSIZE 32
//#define randf (*rando)

unsigned int (* randf)(int,int);

pthread_cond_t *notFull, *notEmpty;
pthread_mutex_t *mootex;

int randomseed(){
  struct timeval tv;

  gettimeofday(&tv, NULL);

  unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
	(unsigned long long)(tv.tv_usec) / 1000;
  /* thanks https://stackoverflow.com/a/10889489 */

  return millisecondsSinceEpoch;
}

unsigned int randomint_mt(int low, int high){
  init_genrand(randomseed());
  return (abs(genrand_int32()) % (high+1 - low) + low);
}

unsigned int randomint_rd(int low, int high){
  unsigned int rnum;
  __asm__ ("rdrand %[rnum]" 
	: [rnum] "=r" (rnum)
	: 
	: "cc");
  /* thanks https://codereview.stackexchange.com/a/150230 */
  return (rnum % (high+1 - low) + low);
}

// dumps random numbers into a buffer
// waits 3-7 seconds between productions
void *run_producer(void *buff){
  printf("Producer created!"); 

  do {
    printf("producer running\n");
    
    struct num numb;
    numb.val = (*randf)(1,100);
    numb.time = (*randf)(2,9);
    push(buff,numb);
    sleep((*randf)(3,7));
  } while (1 == 1);
  return (NULL);
}

// prints numbers dumped into buffer by producer
// waits 2-9 seconds between consumptions
void *run_consumer(void *buff){
  printf("Consumer created!");
  //pthread_join(pthread_self(), NULL);
    printf("consumer running\n");
  while (!isEmpty(buff)){
      struct num n = pop(buff);
      printf("%d\n", n.val);
      sleep(n.time);
  }
  return (NULL);
}


int main(int argc, char **argv)
{

  	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	
	//unsigned int rnum;
	char vendor[13];
	unsigned int l = 3;
	unsigned int h = 7;							
	
	unsigned int (*rando)(int, int);
	struct Stack *buffer = createStack(BUFFSIZE);

	/* thanks https://docs.oracle.com/cd/E19253-01/816-5137/tlib-95520/index.html */
	pthread_t tidc;
	pthread_t tidp;
	void *arg;
	int ret;
	int *status;
	
	eax = 0x01;
	
	__asm__ __volatile__(
		"cpuid;"
		: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
		: "a"(eax)
		);
										
	if(ecx & 0x40000000){
		//use rdrand
		printf("Using rdrand!\n");
		//printf("%d\n", randomint_rd(1,10));
		randf = &randomint_rd;
	}
	else{
		//use mt19937
		printf("Using Mersenne Twister!\n");
  		//printf("%d\n", randomint_mt(3,7));
		randf = &randomint_mt;
	}

	//printf("%d\n", (* randf)(3,7));
	
	// create a producer
	pthread_create(&tidp, NULL, run_producer, buffer);
	pthread_detach(tidp);

	// create a consumer
	pthread_create(&tidc, NULL, run_consumer, buffer);
	pthread_detach(tidc);
	

	//exit(0);
	// suicide
	// ...
	return 0;
}


