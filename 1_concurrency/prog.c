#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "./mt/mt19937ar.h"

int randomseed(){
  struct timeval tv;

  gettimeofday(&tv, NULL);

  unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
	(unsigned long long)(tv.tv_usec) / 1000;
  /* thanks https://stackoverflow.com/a/10889489 */

  return millisecondsSinceEpoch;
}

int randomint_mt(int low, int high){
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
	
	eax = 0x01;

	__asm__ __volatile__(
		"cpuid;"
		: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
		: "a"(eax)
		);
										
	if(ecx & 0x40000000){
		//use rdrand
		printf("Using rdrand!\n");
		printf("%d\n", randomint_rd(1,10));
	}
	else{
		//use mt19937
		printf("Using Mersenne Twister!\n");
  		printf("%d\n", randomint_mt(3,7));
	}
	
	return 0;
}


