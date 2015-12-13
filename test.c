#include "lfq.h"
#include <pthread.h>

volatile int cond = 0;

void *consumer(void *_queue){
    queue_t *queue = (queue_t *)_queue;
    int i=100;
    for(;;) {
           qpop(queue,(int)pthread_self());
           sched_yield();
           __sync_synchronize();
           if (__sync_bool_compare_and_swap(&cond,1,1)){
            printf("\nquiting\n");
           	break;
          }
	}
}

int main(){

	pthread_t _thread;
	pthread_t _thread2;
	queue_t *queue = q_initialize();
	pthread_create(&_thread,NULL,consumer,queue);
	pthread_create(&_thread2,NULL,consumer,queue);

	int i = 0;

	for(i=1500;i>0;i--) { qpush(queue,i); }

	sleep(10);
	__sync_bool_compare_and_swap(&cond,0,1);

	pthread_join(_thread,NULL);
	pthread_join(_thread2,NULL);

	return 0;
}
