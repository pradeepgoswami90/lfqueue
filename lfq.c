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

#ifndef LFQ
# include "lfq.h"
#endif

queue_t *
q_initialize() {

  queue_t *queue = (queue_t *) malloc(sizeof(queue_t)) MPANIC(queue);
  node_t *node = (node_t *) malloc(sizeof(node_t)) MPANIC(node);
  pointer_t *nodeptr = (pointer_t *) malloc(sizeof(pointer_t)) MPANIC(nodeptr);

  node->next = NULL;
  node->value = NULL;
  nodeptr->count = 1;
  nodeptr->nptr = node;
  queue->head = queue->tail = nodeptr;

 /**************************************
 * head ..,    .----.   .----.
 *        |->  |nptr|-->|node|->[ptr]NULL
 * tail ..'    `----'   `----'
 ***************************************/
  return queue;
}

void
qpush(queue_t *queue,void *value) {

  pointer_t *head;
  pointer_t *tail;
  pointer_t *next;

  node_t *node = (node_t *) malloc(sizeof(node_t)) MPANIC(node);
//  if( node == NULL ) { printf("\n MALLOC FAILED \n"); exit(1); }

  pointer_t * nodeptr = (pointer_t *) malloc(sizeof(pointer_t)) MPANIC(nodeptr);
//  if( nodeptr == NULL ) { printf("\n MALLOC FAILED \n"); exit(1); }

  node->value = value;
  node->next = NULL;
  nodeptr->count = 1;
  nodeptr->nptr = node;

  while(1){
    head = queue->head;
    tail = queue->tail;
    next = tail->nptr->next;

    if (tail == queue->tail) {
      if(next == NULL){
	__sync_fetch_and_add(&(nodeptr->count),1);
	if (__sync_bool_compare_and_swap(&(tail->nptr->next),(long long unsigned int)next,nodeptr)){
	  if (__sync_bool_compare_and_swap(&(queue->tail),(long long unsigned int)tail,nodeptr)){
	    printf("\n did this \n");
	    break;
	  }
	}
      } else {
	printf("\n wrote %d\n",__sync_fetch_and_add(&(next->count),1));
	if (__sync_bool_compare_and_swap(&(queue->tail),(long long unsigned int)tail,next->nptr)){
	}
      }
    }
 }

}

value_t *
qpop(queue_t *queue,int thrd){

  pointer_t *head = NULL;
  pointer_t *tail = NULL;
  pointer_t *_next = NULL;
  pointer_t *tmp = NULL;
  value_t *val = NULL;

  while (1){
    head = queue->head;
    tail = queue->tail;

    if(head == tail) return NULL;

    tmp = (pointer_t *)queue->head->nptr->next;
    _next = queue->head->nptr->next;
    int count = head->count;

    if (_next == NULL) return NULL;

    if (head == queue->head){

  	  if (head->nptr == tail->nptr){
				if (_next == NULL) return NULL;

	__sync_fetch_and_add(&(_next->count),1);
	if (!__sync_bool_compare_and_swap(&(queue->tail),(long long unsigned int)tail,_next)) continue;

      } else {

	val = _next->nptr->value;
	__sync_fetch_and_add(&(_next->count),1);
	if (__sync_bool_compare_and_swap(&(queue->head->nptr->next),(long long unsigned int)tmp,_next->nptr->next)) {
	  __sync_synchronize();
	  break;
	}
      }
    }
  }

  if (val != NULL){
    //    if (tmp != NULL){
    //      __sync_fetch_and_add(&(tmp->count),1);
    //      if (__sync_bool_compare_and_swap(&tmp->count,tmp->count,tmp->count)){
    //        _next->nptr->value = ;
        free(_next->nptr);		
        free(_next);

    return val;
	//      }
	//    }    
  }

  return NULL;
  
}
