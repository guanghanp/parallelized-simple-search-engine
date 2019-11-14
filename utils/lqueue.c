/* 
 * lqueue.c -- implementation of a queue
 * 
 */

#include <lqueue.h>
#include <pthread.h>
#include <unistd.h>

typedef struct lq_t{
	queue_t* qp;
	pthread_mutex_t m;
}lq_t;

/* create an empty queue */
lqueue_t* lqopen(void){
	lq_t *lq;
	if(!(lq = (lq_t*)malloc(sizeof(lq_t)))){
		printf("Malloc failure\n");
	}
	lq->qp = qopen();
	pthread_mutex_init(&(lq->m),NULL);
	return (lqueue_t*)lq;
}

/* deallocate a queue, frees everything in it */
void lqclose(lqueue_t *lqp){
	lq_t* lq = (lq_t*)lqp;
	qclose(lq->qp);
	pthread_mutex_destroy(&(lq->m));
	free(lq);
}

/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t lqput(lqueue_t *lqp, void *elementp){
	lq_t *lq=(lq_t*)lqp;
	int32_t output;
	pthread_mutex_lock(&(lq->m));
	//sleep(5);
	//printf("snowing \n");
	output = qput(lq->qp,elementp);
	pthread_mutex_unlock(&(lq->m));
	return output;
}

/* get the first first element from queue, removing it from the queue */
void* lqget(lqueue_t *lqp){
	lq_t *lq=(lq_t*)lqp;
	void* element;
	pthread_mutex_lock(&(lq->m));
	element = qget(lq->qp);
	pthread_mutex_unlock(&(lq->m));
	return element;
}

/* apply a function to every element of the queue */
void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)){
	lq_t *lq=(lq_t*)lqp;
	pthread_mutex_lock(&(lq->m));
	qapply(lq->qp,fn);
	pthread_mutex_unlock(&(lq->m));
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
void* lqsearch(lqueue_t *lqp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp){
	lq_t *lq=(lq_t*)lqp;
	void* output;
	pthread_mutex_lock(&(lq->m));
	output = qsearch(lq->qp,searchfn,skeyp);
	pthread_mutex_unlock(&(lq->m));
	return output;
}

/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* lqremove(lqueue_t *lqp,
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp){
	lq_t *lq=(lq_t*)lqp;
	void* output;
	pthread_mutex_lock(&(lq->m));
	output = qremove(lq->qp,searchfn,skeyp);
	pthread_mutex_unlock(&(lq->m));
	return output;
}

/* concatenatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
 */
void lqconcat(lqueue_t *lq1p, lqueue_t *lq2p){
	lq_t *lq1=(lq_t*)lq1p;
	lq_t *lq2=(lq_t*)lq2p;
	pthread_mutex_lock(&(lq1->m));
	pthread_mutex_lock(&(lq2->m));
	qconcat(lq1->qp,lq2->qp);
	pthread_mutex_unlock(&(lq1->m));
	pthread_mutex_unlock(&(lq2->m));
}
