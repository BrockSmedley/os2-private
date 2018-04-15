#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "./mt/mt19937ar.h"

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
		printf("You gotta use rdrand!\n");
	}
	else{
		//use mt19937
  		printf("random: %d\n", randomint_mt(3,7));
	}
	
	return 0;
}

int randomseed(){
  struct timeval tv;

  gettimeofday(&tv, NULL);

  unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
	(unsigned long long)(tv.tv_usec) / 1000;

  return millisecondsSinceEpoch;
}

int randomint_mt(int low, int high){
  init_genrand(randomseed());
  return (abs(genrand_int32()) % (high+1 - low) + low);
}
