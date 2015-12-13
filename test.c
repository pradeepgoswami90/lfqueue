/*
Copyright (c) 2015, Mike Taghavi (mitghi) <mitghi@me.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
