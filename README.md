# lfqueue
Simple Lock-Free Queue implemented in C based on [this paper](https://www.research.ibm.com/people/m/michael/podc-1996.pdf).

# Example
    #include <stdio.h>
    #include <stdlib.h>
    #include <pthread.h>
    
    #include "lfq.h"
    
    #define CHAR_TYPE 0
    
    volatile int cond = 0;
    
    void *consumer(void *_queue){
      Queue *queue = (Queue *)_queue;
    
      int i=100;
      Value *value = NULL;
    
      for(;;) {
        value = qpop(queue,(unsigned int)pthread_self());
    
        if(value != NULL && value->data != NULL){
          if (value->type == CHAR_TYPE) {
    	printf("\n %s, %u\n", (char *)value->data, (unsigned int)pthread_self());
          }
        }
    
        sched_yield();
        value = NULL;
        CHECK_COND(cond);
      }
    }
    
    int main(){
      int i = 0;
    
      Queue *queue = q_initialize();
    
      pthread_t _thread;
      pthread_t _thread2;
    
      pthread_create(&_thread,NULL,consumer,queue);
      pthread_create(&_thread2,NULL,consumer,queue);
    
      Value *value = malloc(100 * sizeof(Value));
    
      for(i = 0; i < 100; i++){
        value[i].type = CHAR_TYPE;
        value[i].data = (char *) malloc(8* sizeof(char *));
        sprintf(value[i].data,"test %d.",i);
        qpush(queue,&value[i]);
      }
    
      sleep(1);
    
      for (i=1; i < 90; i++){
        Value *t = &value[i];
        free(t->data);
      }
    
      free(value);
      queue_free(queue);
      
      __sync_bool_compare_and_swap(&cond,0,1);
    
      pthread_join(_thread,NULL);
      pthread_join(_thread2,NULL);
    
      return 0;
    }

# TODOs
1. Memory Pool.
2. Hazard Pointer.
