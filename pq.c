/*
 * In this file, you will write the structures and functions needed to
 * implement a priority queue.  Feel free to implement any helper functions
 * you need in this file to implement a priority queue.  Make sure to add your
 * name and @oregonstate.edu email address below:
 *
 * Name: Alex Grejuc
 * Email: grejuca@oregonstate.edu
 */

#include <stdlib.h>
#include <stdio.h>
#include "dynarray.h"
#include "pq.h"


/*
 * a node of the priority queue - stores priority value and data
 */
struct pq_node{
	void* val; //data associated w/ node
	int priority; //priority, where low val means high priority 
};

/*
 * creates a pq_node by initializing it with values
 *
 * val - the data being assigned to the node
 * priority - the priority value of the node, used to determine position in heap
 */
struct pq_node* pq_node_create(void* val, int priority){
	struct pq_node* node = malloc(sizeof(struct pq_node));

	node->val = val;
	node->priority = priority;
};

/*
 * This is the structure that represents a priority queue.  You must define
 * this struct to contain the data needed to implement a priority queue.
 */
struct pq{
	int last; //last inserted value
	int next; //location to insert the next value

	struct dynarray* arr; 
};


/*
 * This function should allocate and initialize an empty priority queue and
 * return a pointer to it.
 */
struct pq* pq_create() {

  struct pq* pq = malloc(sizeof(struct pq));
  pq->arr = dynarray_create();

  pq->last = -1; //incremented after each insertion, will be 0 only after node inserted
  pq->next = 0; 

  return pq; 
}


/*
 * This function should free the memory allocated to a given priority queue.
 * Note that this function SHOULD NOT free the individual elements stored in
 * the priority queue.  That is the responsibility of the caller.
 *
 * Params:
 *   pq - the priority queue to be destroyed.  May not be NULL.
 */
void pq_free(struct pq* pq) {

	//free each individual node (but not the values it holds -- these are to be freed by user) 
	for(int i = 0; i < dynarray_size(pq->arr); i++){
		free(dynarray_get(pq->arr, i));
	}

	dynarray_free(pq->arr);
	free(pq);	
}


/*
 * This function should return 1 if the specified priority queue is empty and
 * 0 otherwise.
 *
 * Params:
 *   pq - the priority queue whose emptiness is to be checked.  May not be
 *     NULL.
 *
 * Return:
 *   Should return 1 if pq is empty and 0 otherwise.
 */
int pq_isempty(struct pq* pq) {
  return !dynarray_size(pq->arr);
}


/*
 * percolates values up the heap to ensure newly inserted values are in correct
 * spot. Maintains minimizing heap order. 
 *
 * idx - starting index of the value to be percolated
 */
void perc_up(struct dynarray* arr, int idx){
	int parent = (idx - 1) / 2;
	
	void* temp = dynarray_get(arr, parent);

	//while parent < current spot 
	while(parent >= 0 && ((struct pq_node*)dynarray_get(arr, parent))->priority > ((struct pq_node*)dynarray_get(arr, idx))->priority ){
		
		//swap parent with current
		dynarray_set(arr, parent, dynarray_get(arr, idx) );
		dynarray_set(arr, idx, temp); 

		//update indexes 
		idx = parent; 
		parent = (idx - 1) / 2;
		
		temp = dynarray_get(arr, parent);
	}
}

/*
 * This function should insert a given element into a priority queue with a
 * specified priority value.  Note that in this implementation, LOWER priority
 * values are assigned to elements with HIGHER priority.  In other words, the
 * element in the priority queue with the LOWEST priority value should be the
 * FIRST one returned.
 *
 * Params:
 *   pq - the priority queue into which to insert an element.  May not be
 *     NULL.
 *   value - the value to be inserted into pq.
 *   priority - the priority value to be assigned to the newly-inserted
 *     element.  Note that in this implementation, LOWER priority values
 *     should correspond to elements with HIGHER priority.  In other words,
 *     the element in the priority queue with the LOWEST priority value should
 *     be the FIRST one returned.
 */
void pq_insert(struct pq* pq, void* value, int priority) {
	
	struct pq_node* node = pq_node_create(value, priority);	
	dynarray_insert(pq->arr, pq->next, node);
	
	perc_up(pq->arr, pq->next); //percolate value up heap to maintain minimizing order
	
	pq->last++;
	pq->next++;
}


/*
 * This function should return the value of the first item in a priority
 * queue, i.e. the item with LOWEST priority value.
 *
 * Params:
 *   pq - the priority queue from which to fetch a value.  May not be NULL or
 *     empty.
 *
 * Return:
 *   Should return the value of the first item in pq, i.e. the item with
 *   LOWEST priority value.
 */
void* pq_first(struct pq* pq) {	
  return ((struct pq_node*)dynarray_get(pq->arr, 0))->val;
}


/*
 * This function should return the priority value of the first item in a
 * priority queue, i.e. the item with LOWEST priority value.
 *
 * Params:
 *   pq - the priority queue from which to fetch a priority value.  May not be
 *     NULL or empty.
 *
 * Return:
 *   Should return the priority value of the first item in pq, i.e. the item
 *   with LOWEST priority value.
 */
int pq_first_priority(struct pq* pq) {
  return ((struct pq_node*)dynarray_get(pq->arr, 0))->priority;
}

int get_min_child(struct dynarray* arr, int idx){
	int lchild, rchild, minchild; 
	int size = dynarray_size(arr); 

	lchild = (idx * 2) + 1;

	if(lchild >= size) {
		return -1; //both children guaranteed out of bounds 
	}
	else{
		rchild = lchild + 1;
	}

	if(rchild >= size){
		minchild = lchild; 
	}
	else{
		if( ((struct pq_node*)dynarray_get(arr, lchild))->priority > ((struct pq_node*)dynarray_get(arr, rchild))->priority ){
			minchild = rchild; 
		}
		else {
			minchild = lchild; 
		}
	}

	return minchild; 
}

/*
 * percolates values down heap to ensure newly inserted values are in correct
 * spot. Maintains minimizing heap order. 
 *
 * idx - starting index of the value to be percolated
 */
void perc_down(struct dynarray* arr, int idx){
	int minchild;

	struct pq_node* temp; 

	int minchild_priority, idx_priority; 


	while(1){
		minchild = get_min_child(arr, idx); 
	
		if(minchild == -1) break; //if out of bounds of array (node is a leaf)

		//store priority vals of each node for readability
		minchild_priority = ((struct pq_node*)dynarray_get(arr, minchild))->priority;
	   	idx_priority = 	((struct pq_node*)dynarray_get(arr, idx))->priority;

		if(minchild_priority < idx_priority){
			
			temp = dynarray_get(arr, minchild); 

			//swap minchild node w/ current node
			dynarray_set(arr, minchild, dynarray_get(arr, idx));
			dynarray_set(arr, idx, temp); 

			idx = minchild;
		}
		else{ //minchild >= current node
			break; 
		}

	}
}

/*
 * This function should return the value of the first item in a priority
 * queue, i.e. the item with LOWEST priority value, and then remove that item
 * from the queue.
 *
 * Params:
 *   pq - the priority queue from which to remove a value.  May not be NULL or
 *     empty.
 *
 * Return:
 *   Should return the value of the first item in pq, i.e. the item with
 *   LOWEST priority value.
 */
void* pq_remove_first(struct pq* pq) {
  	void* first = ((struct pq_node*)dynarray_get(pq->arr, 0))->val; //store val so it can be returned 
	free(dynarray_get(pq->arr, 0));
	
	dynarray_set(pq->arr, 0, dynarray_get(pq->arr, pq->last)); //place last inserted val at top of heap
	dynarray_remove(pq->arr, pq->last); 
	
	perc_down(pq->arr, 0); //percolate val that was inserted at top to maintain order
   	
	pq->last--;
	pq->next--;
	
	return first;
}
