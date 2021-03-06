#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define LOCKSTEPTHREAD_IMPL
#include "lockstepthread.h"

#define NWORKERS 20
#define SQRT_COUNT (NWORKERS * 10000)
//Multipurpose macro.
#define MTXSTAT(var, val, mtx) pthread_mutex_lock(&mtx); var = val; pthread_mutex_unlock(&mtx);
pthread_mutex_t wMtx[NWORKERS];
int wStatuses[NWORKERS] = {0};

double sqrtData[SQRT_COUNT];

//if you don't want to duplicate code, you can use arg.

void workerFuncSqrt(void* arg){
	size_t id = ((size_t)arg);
	//We have to determine where in the array we're writing it.
	size_t off = id * SQRT_COUNT/NWORKERS;
	for(size_t i = 0; i < (SQRT_COUNT / NWORKERS) && (i+off < SQRT_COUNT); i++)
		sqrtData[i+off] = sqrt((double)(i+off));
	MTXSTAT(wStatuses[id], 2, wMtx[id]);
}



int main(){
	lsthread workers[NWORKERS];
	for(int i = 0; i < NWORKERS; i++)
		{init_lsthread(workers+i);start_lsthread(workers+i);}
	for(int i = 0; i < NWORKERS; i++)
		wMtx[i] = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	for(size_t i = 0; i < NWORKERS; i++)
		{workers[i].argument = (void*)i;workers[i].execute = workerFuncSqrt;}
	
	
	//RELEASE THE KRAKENS!
	for(int i = 0; i < NWORKERS; i++)
		step(workers+i);
	int numReady = 0;
	while(numReady < NWORKERS)
		for(int i = 0; i < NWORKERS; i++){
			int a = 0;
			MTXSTAT(a, wStatuses[i], wMtx[i]);
			if(a == 2){
				numReady++;
				printf("%d is ready!\n", i);
				lock(workers+i);
				wStatuses[i] = 0; //The thread is not active anymore we don't have to lock.
				//We could add this thread to a pool of idle, available workers now, change its execute function when we needed,
				//and do the exact same thing again!
			}
		}
	for(int i = 0; i < SQRT_COUNT; i++)
		printf("sqrt(%d) = %f\n",i,sqrtData[i]);
	for(int i = 0; i < NWORKERS; i++)
		{kill_lsthread(workers+i);destroy_lsthread(workers+i);}			
}
