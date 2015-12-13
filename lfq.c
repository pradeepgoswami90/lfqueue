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

queue_t * q_initialize() {
	queue_t *queue = (queue_t *) malloc(sizeof(queue_t));
	 node_t *node = (node_t *) malloc(sizeof(node_t));
	 pointer_t * nodeptr = (pointer_t *) malloc(sizeof(pointer_t));

	node->next = NULL;
	nodeptr->count = 1;
	nodeptr->nptr = node;
	queue->head = queue->tail = nodeptr;

	/**************************************
	* head ..,    .----.   .----.
	* 		 |->  |nptr|-->|node|->[ptr]NULL
	* tail ..'	  `----'   `----'
	***************************************/
	return queue;
}

void
qpush(queue_t *queue,int value)
{

	 pointer_t *head;
	 pointer_t *tail;
	 pointer_t *next;

	node_t *node = (node_t *) malloc(sizeof(node_t));
	if( node == NULL ) { printf("\n MALLOC FAILED \n"); exit(1); }
	pointer_t * nodeptr = (pointer_t *) malloc(sizeof(pointer_t));
	if( nodeptr == NULL ) { printf("\n MALLOC FAILED \n"); exit(1); }

	node->value = value;
	node->next = NULL;
	nodeptr->count = 1;
	nodeptr->nptr = node;

	while(1){
		head = queue->head;
		tail = queue->tail;
		next = tail->nptr->next;

		if (tail == queue->tail)
		{
			if(next == NULL){
				__sync_fetch_and_add(&(nodeptr->count),1);
				if (__sync_bool_compare_and_swap(&(tail->nptr->next),(long long unsigned int)next,nodeptr)){
					__sync_bool_compare_and_swap(&(queue->tail),(long long unsigned int)tail,nodeptr);
					break;
				}
			} else {
				PR(printf("\ninside this\n")); // got it ?
				printf("\n wrote %d\n",__sync_fetch_and_add(&(next->count),1));
				__sync_bool_compare_and_swap(&(queue->tail),(long long unsigned int)tail,next->nptr);
			}
		}
	}
}


void qpop(queue_t *queue,int thrd){

	pointer_t *head=NULL;
	pointer_t *tail=NULL;
	volatile pointer_t *_next=NULL;
	pointer_t *tmp=NULL;
	int val = -1;
	while (1){
		head = queue->head;
		tail = queue->tail;
		if(head == tail) return;
		tmp = queue->head->nptr;
		_next = queue->head->nptr->next;
		int count = head->count;
		if (_next == NULL) return;

		if (head == queue->head){

			if (head->nptr == tail->nptr){
				if (_next == NULL) return;
				__sync_fetch_and_add(&(_next->count),1);
				__sync_bool_compare_and_swap(&(queue->tail),(long long unsigned int)tail,_next);
				printf("\n here \n"); // seriously
				return;
			} else{
				val = _next->nptr->value;
				__sync_fetch_and_add(&(_next->count),1);
				if (__sync_bool_compare_and_swap(&(queue->head->nptr),(long long unsigned int)head->nptr,_next->nptr)) {
					__sync_synchronize();
					break ;
				}
			}
		}
	}

if (val != -1){
	PR(printf("\nQueue pop: %d, thread %d\n",val,thrd));
		if (tmp != NULL){
			__sync_fetch_and_add(&(tmp->count),1);
			if (__sync_bool_compare_and_swap(&tmp->count,tmp->count,tmp->count)){
				free(tmp);
				printf("\n memory freed, val: %d \n",val);
			}
		}
	}
}
