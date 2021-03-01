#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define LOCKSTEPTHREAD_IMPL
#include "lockstepthread.h"
int var_t2 = 0;
int var_t1 = 0;

void thread1_func(){
	for(int i = 0; i < 100000000; i++)
		if(i == 300)
			var_t1++;
	printf("Thread1_func, %d\n", var_t1);
}

void thread2_func(){
	for(int i = 0; i < 1000; i++)
		if(i == 300)
			var_t2++;
	printf("Thread2_func, %d\n", var_t2);
}

int main(int argc, char** argv){
	lsthread t1, t2;
	init_lsthread(&t1);
	init_lsthread(&t2);
	t1.execute = &thread1_func;
	t2.execute = &thread2_func;
	start_lsthread(&t1);
	start_lsthread(&t2);
//	if(0)
	for(int i = 0; i < 30; i++){

		lock(&t1);
		lock(&t2);

		step(&t1);
		step(&t2);

	}
	kill_lsthread(&t1);
	kill_lsthread(&t2);
	destroy_lsthread(&t1);
	destroy_lsthread(&t2);
	return 0;
}
