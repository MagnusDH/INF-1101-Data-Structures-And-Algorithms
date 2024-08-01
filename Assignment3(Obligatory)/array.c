#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "list.h"
#include "set.h"

#define MAXSIZE 2000

/*The type of sets*/
typedef struct set set_t;
struct set{
    void **array;
    int size;
    int maxsize;
    cmpfunc_t cmpfunc;
};

/*Creates a new set using the given comparison function
 *to compare elements of the set*/
set_t *set_create(cmpfunc_t cmpfunc)
{
    /*Allocating space for set*/
    set_t *set = malloc(sizeof(set_t));
    if(set == NULL){
        return NULL;
    }

    /*Initializing set properties*/
    set->size = 0;
    set->maxsize = MAXSIZE;
    set->cmpfunc = cmpfunc;
    /*Allcoating space for array*/
    set->array = malloc(sizeof(void*) * set->maxsize);
    if(set->array == NULL){
        return NULL;
    }

    return set;
}

/*Destroys the given set*/
void set_destroy(set_t *set)
{
    /*Deleting array and set*/
    free(set->array);
    free(set);
}

/*Returns the size (cardinality) of the given set*/
int set_size(set_t *set)
{
    return set->size;
}

/*Adds the given element to the given set*/
void set_add(set_t *set, void *elem)
{   
    set->maxsize = MAXSIZE;

    /*Checking if there is enough space in array*/
    if(set->size >= (set->maxsize/2) ){
        set->maxsize = set->maxsize * 2;
        set->array = realloc(set->array, sizeof(void*) * set->maxsize);
        if(set->array == NULL){
            return;
        }
    }
    
    /*If set does not already contain given elemet, add it to set*/
    if(set_contains(set, elem) == 0){
        set->array[set->size] = elem;
        set->size++;
    }
}

/*Returns 1 if the given element is contained in
 *the given set, 0 otherwise*/
int set_contains(set_t *set, void *elem)
{
    /*Creating iterator for set*/
    set_iter_t *iterator = set_createiter(set);

    /*Checking if given element does not exist in array*/
    while(set_hasnext(iterator) != 0){
        void *item = set_next(iterator);
        if(set->cmpfunc(item, elem) == 0){
            return 1;
        }
    }

    return 0;
}

/*Reverses the order of a given set*/
set_t *reverse_set(set_t *set)
{
}

/*Returns the union of the two given sets; the returned
 *set contains all elements that are contained in either a or b*/
set_t *set_union(set_t *a, set_t *b)
{
    set_t *union_set = set_copy(b);
    set_iter_t *iterator = set_createiter(a);
    void *elem_a = set_next(iterator);

    while(elem_a != NULL){
        if(set_contains(union_set, elem_a) == 0){
            set_add(union_set, elem_a);
            elem_a = set_next(iterator);
        }
        else{
            elem_a = set_next(iterator);
        }
    }
    set_destroyiter(iterator);
    return union_set;
}

/*Returns the intersection of the two given sets; the
 *returned set contains all elements that are contained
 *in both a and b*/
set_t *set_intersection(set_t *a, set_t *b)
{
    set_t *intersection_set = set_create(a->cmpfunc);
    set_iter_t *iter_a = set_createiter(a);
    void *elem_a = set_next(iter_a);

    while(elem_a != NULL){
        if(set_contains(b, elem_a) == 1){
            set_add(intersection_set, elem_a);
            elem_a = set_next(iter_a);
        }
        else{
            elem_a = set_next(iter_a);
        }
    }
    set_destroyiter(iter_a);

    return intersection_set;
}

/*Returns the set difference of the two given sets; the
 *returned set contains all elements that are contained
 *in a but not those who are equal to ones in b*/
set_t *set_difference(set_t *a, set_t *b)
{
    set_t *difference_set = set_create(a->cmpfunc);
    set_iter_t *iter_a = set_createiter(a);
    void *elem_a = set_next(iter_a);

    while(elem_a != NULL){
        if(set_contains(b, elem_a) == 0){
            set_add(difference_set, elem_a);
            elem_a = set_next(iter_a);
        }
        else{
            elem_a = set_next(iter_a);
        }
    }
    set_destroyiter(iter_a);

    return difference_set;
}

/*Returns a copy of the given set*/
set_t *set_copy(set_t *set)
{
    set_t *copy = set_create(set->cmpfunc);

    set_iter_t *iterator = set_createiter(set);

    while(set_hasnext(iterator) != 0){
        void *item = set_next(iterator);
        set_add(copy, item);
    }

    set_destroyiter(iterator);

    return copy;
}

/*The type of set iterators*/
typedef struct set_iter set_iter_t;
struct set_iter{
    set_t *set;
    int index;
};

/*Creates a new set iterator for iterating over the given set*/
set_iter_t *set_createiter(set_t *set)
{
    set_iter_t *iterator = malloc(sizeof(set_iter_t));
    if(iterator == NULL){
        return NULL;
    }

    iterator->set = set;
    iterator->index = 0;

    return iterator;
}

/*Destroys the given set iterator*/
void set_destroyiter(set_iter_t *iter)
{
    free(iter);
}

/*Returns 0 if the given set iterator has reached the end of the
 *set, or 1 otherwise*/
int set_hasnext(set_iter_t *iter)
{   
    /*Checking if reached end of array/iter*/
    if(iter->index >= iter->set->size){
        return 0;
    }
    else{
        return 1;
    }
}

/*Returns the next element in the sequence represented by the given
 *set iterator*/
void *set_next(set_iter_t *iter)
{
    if(iter->index >= iter->set->size){
        return NULL;
    }
    else{
        void *elem = iter->set->array[iter->index];
        iter->index++;
        return elem;
    }
}