#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#ifndef LFQ
#define LFQ
#endif

#define RETDISCARD /* VALUE IS DISCARDED */

#define PR(a) a;

typedef struct _node_t node_t;
typedef struct _pointer_t pointer_t;
typedef struct _queue_t queue_t;

struct _node_t{
	 unsigned int value;
	 pointer_t *next;

};

struct _pointer_t{
	 int count;
	 node_t *nptr;
};

struct _queue_t{
	 pointer_t *head;
	 pointer_t *tail;
};

queue_t *
q_initialize(void);


void
qpush(queue_t *, int);

void
qpop(queue_t *,int);


