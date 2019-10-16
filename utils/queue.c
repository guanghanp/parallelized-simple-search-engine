/* 
 * queue.c -- implementation of a queue
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

typedef struct node_t{
	void* element;
	struct node_t *next;
}node_t;

typedef struct q_t{
	node_t *front;
	node_t *back;
}q_t;


/* create an empty queue */
queue_t* qopen(void){
	q_t *q;
	if(!(q = (q_t*)malloc(sizeof(q_t)))){
		printf("Malloc failure\n");
	}
	q->front=NULL;
	q->back=NULL;
	return (queue_t*)q;
}

/* deallocate a queue, frees everything in it */
void qclose(queue_t *qp){
	q_t* q = (q_t*)qp;
	node_t* f=NULL;
	
	for (node_t *p=q->front;p!=NULL;f=p,p=p->next){
		if(f!=NULL){
			free(f);
		}
	}
	free(f);
	free(q);
}

/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t qput(queue_t *qp, void *elementp){
	node_t* node;
	q_t *q=(q_t*)qp; 
	if(!(node = (node_t*)malloc(sizeof(node_t)))){
		printf("Malloc failure\n");
		return 1;
	}
	node->element=elementp;
	node->next=NULL;
	if(q->front==NULL)
		q->front=node;
	else
		q->back->next=node;
	q->back=node;
	return 0;
}

/* get the first first element from queue, removing it from the queue */
void* qget(queue_t *qp){
	q_t *q=(q_t*)qp;
	node_t *first = q->front;
	if(first == NULL)
		return NULL;
	void *element = first->element;
	if(q->front == NULL)
		q->back = NULL;
	q->front = q->front->next;
	free(first);
	return element;
}

/* apply a function to every element of the queue */
void qapply(queue_t *qp, void (*fn)(void* elementp)){
	node_t *p;
	q_t *q=(q_t*)qp;
	for (p=q->front;p!=NULL;p=p->next){
		fn(p->element);
	}
}

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* qsearch(queue_t *qp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp){
	node_t *p;
	q_t *q=(q_t*)qp;
	for (p=q->front;p!=NULL;p=p->next){
		if (searchfn(p->element, skeyp) == true){
			return p->element;
		}
	}
	return NULL;
}

/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* qremove(queue_t *qp,
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp){
	node_t *p;
	void *element;
	node_t *f=NULL;
	q_t *q=(q_t*)qp;
	for (p=q->front;p!=NULL;f=p,p=p->next){
		if (searchfn(p->element, skeyp) == true)
			break;
	}

	if(p == NULL)
		return NULL;
	
	if (f == NULL)
		q->front = p->next;
	else
		f->next=p->next;

	if (p == q->back)
		q->back=f;

	element=p->element;
	free(p);
	return element;
}

/* concatenatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
 */
void qconcat(queue_t *q1p, queue_t *q2p){
	q_t* q1 = (q_t*)q1p;
	q_t* q2 = (q_t*)q2p;
	
	if(q1->front==NULL)
		q1->front=q2->front;
	else
		q1->back->next = q2->front;
		
	q1->back = q2->back;	
	free(q2);
}
