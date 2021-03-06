#ifndef TPOOL_H
#define TPOOL_H
#include "lockstepthread.h"

#define ASSERT_SAME_TYPE(a, b)  ((&(a) == &(b)))

//A has B of type C, testname D.
#define HAS_ITEM(A,B,C,D) \
static inline C* buildt_##A##B##D(){ \
  A t; \
  C q;\
  int f = ASSERT_SAME_TYPE(q, t.B);\
  C* ret = &( t . B );\
  return ret;\
}

#define FUNC_DUCKTYPE(retval, func,  testname, ...)\
typedef retval (*buildtt_##testname)(__VA_ARGS__);\
static inline void buildt_##testname(){\
    buildtt_##testname b = func;\
    return;\
}

#define MTXSTAT(var, val, mtx) pthread_mutex_lock(&mtx); var = val; pthread_mutex_unlock(&mtx);

#define TPOOL(name, TPOOL_NWORKERS, argstruct)									\
HAS_ITEM(argstruct, tid, size_t, pool_argstruct_has_tid)					\
/*Must be made extern*/														\
pthread_mutex_t name##wMtx[TPOOL_NWORKERS];										\
argstruct name##argstructs[TPOOL_NWORKERS];										\
int name##wStatuses[TPOOL_NWORKERS] = {0};										\
lsthread name##workers[TPOOL_NWORKERS];											\
static inline void name##_init(){											\
	for(size_t i = 0; i < TPOOL_NWORKERS; i++)									\
		{init_lsthread(name##workers+i);start_lsthread(name##workers+i);}	\
	for(size_t i = 0; i < TPOOL_NWORKERS; i++)									\
		name##wMtx[i] = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;			\
	for(size_t i = 0; i < TPOOL_NWORKERS; i++)									\
		{name##workers[i].argument = NULL;name##workers[i].execute = NULL;}		\
}																			\
static inline void name##_destroy(){										\
	for(int i = 0; i < TPOOL_NWORKERS; i++)										\
		{kill_lsthread(name##workers+i);destroy_lsthread(name##workers+i);}	\
}																			\
static inline int name##_get_worker_status(size_t tid){						\
	int r;																	\
	MTXSTAT(r, name##wStatuses[tid], (name##wMtx[tid]));					\
	return r;																\
}																			\
static inline void name##_set_worker_status(size_t tid, int val){			\
	MTXSTAT(name##wStatuses[tid], val, (name##wMtx[tid]));					\
	return;																	\
}																			\
static inline int name##_tryAssignTask(void (*func)(void*), argstruct in){	\
																			\
	for(size_t i = 0; i < TPOOL_NWORKERS; i++){									\
		int a = name##_get_worker_status(i);								\
		if(a == 2 || a==0){													\
			MTXSTAT(name##wStatuses[i],1, name##wMtx[i]);/*TODO: try without mtx*/	\
			lock(name##workers+i);	/*Lock the worker.*/					\
			name##workers[i].argument = name##argstructs + i;/*Setup arg*/	\
			name##argstructs[i] = in;	/*argstruct itself*/				\
			name##argstructs[i].tid = i;/*cont*/							\
			name##workers[i].execute = func;								\
			step(name##workers+i);											\
			return (int)i;													\
		}																	\
	}																		\
	return -1;																\
}																			\
/*End of define.*/

#endif
