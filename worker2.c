#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define LOCKSTEPTHREAD_IMPL
#include "tpool.h"

#define NWORKERS 8
#define NWORKERSPOW 5
#define NWORKERSSQRT 3
#define SQRT_COUNT (NWORKERSSQRT * (1<<15))
#define POW_COUNT (NWORKERSPOW * (1<<15))

double sqrtData[SQRT_COUNT];
double powData[POW_COUNT];

typedef struct{
	size_t tid;
	int work_target;	
}myarg;

//Declare a thread pool of max 20 threads.
TPOOL(mypool, NWORKERS, myarg);

void workerFuncSqrt(void* arg){
	myarg argument = *((myarg*)arg);
	size_t tid = argument.tid;
	//We have to determine where in the array we're writing it.
	size_t off = argument.work_target * SQRT_COUNT/NWORKERSSQRT;
	for(size_t i = 0; i < (SQRT_COUNT / NWORKERSSQRT) && (i+off < SQRT_COUNT); i++)
		sqrtData[i+off] = sqrt((double)(i+off));
	//Indicate that this worker is available for new tasks to be assigned.
	mypool_set_worker_status(tid, 2);
}

void workerFuncPow(void* arg){
	myarg argument = *((myarg*)arg);
	size_t tid = argument.tid;
	//We have to determine where in the array we're writing it.
	size_t off = argument.work_target * POW_COUNT/NWORKERSPOW;
	for(size_t i = 0; i < (POW_COUNT / NWORKERSPOW) && (i+off < POW_COUNT); i++)
		powData[i+off] = pow((double)(i+off),3);
	//Indicate that this worker is available for new tasks to be assigned.
	mypool_set_worker_status(tid, 2);
}


int main(){
	mypool_init();
	int tids[NWORKERSSQRT];
	int tids2[NWORKERSPOW];
	//Assign sqrt workers.
	for(int i = 0; i < NWORKERSSQRT; i++){
		myarg b;
		b.work_target = i;
		do{
			tids[i] = mypool_tryAssignTask(workerFuncSqrt, b);
		}while(tids[i] == -1);
	}
	//Assign pow workers.
	for(int i = 0; i < NWORKERSPOW; i++){
		myarg b;
		b.work_target = i;
		do{
			tids2[i] = mypool_tryAssignTask(workerFuncPow, b);
		}while(tids2[i] == -1);
	}
	//Wait for the sqrt pool to finish
	int foundActive;
	do{
		foundActive = 0;
		for(int i = 0; i < NWORKERSSQRT; i++)
			if(mypool_get_worker_status(tids[i]) == 1)
				foundActive++;
	}while(foundActive != 0);
	//Print out our sqrt values.
	for(int i = 0; i < SQRT_COUNT; i++)
		printf("sqrt(%d) = %f\n",i,sqrtData[i]);
	//Wait for the pow pool to finish.
	do{
			foundActive = 0;
			for(int i = 0; i < NWORKERSSQRT; i++)
				if(mypool_get_worker_status(tids2[i]) == 1)
					foundActive++;
		}while(foundActive != 0);
	for(int i = 0; i < POW_COUNT; i++)
		printf("pow(%d, 3) = %f\n",i,powData[i]);
	mypool_destroy();
	
}
