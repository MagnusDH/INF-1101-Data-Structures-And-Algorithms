#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "list.h"
#include "set.h"

static void printwords(char *prefix, set_t *words)
{
	set_iter_t *it;
	
	it = set_createiter(words);
	printf("%s: ", prefix);
	while (set_hasnext(it)) {
		printf(" %s", (char*)set_next(it)); //THIS WAS ORIGINALY: printf(" %s", set_next(it));
	}
	printf("\n");
	set_destroyiter(it);
}

/*The type of sets*/
typedef struct set set_t;
struct set{
    list_t *list;
    int size;
    cmpfunc_t cmpfunc;
};

/*Creates a new set using the given comparison function
 *to compare elements of the set*/
set_t *set_create(cmpfunc_t cmpfunc)
{
    set_t *set = malloc(sizeof(set_t));
    if(set == NULL){
        printf("Could not create set in set_create function\n");
        return NULL;
    }

    set->size = 0;
    set->list = list_create(cmpfunc);
    set->cmpfunc = cmpfunc;

    return set;
}

/*Destroys the given set*/
void set_destroy(set_t *set)
{
    list_destroy(set->list);
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
    if(set_contains(set, elem) == 0){
        list_addfirst(set->list, elem);
        set->size++;
    }
}

/*Adds the given element to the end of the set*/
void set_addlast(set_t *set, void *elem)
{
    if(set_contains(set, elem) == 0){
        list_addlast(set->list, elem);
        set->size++;
    }
}

/*Returns 1 if the given element is contained in
 *the given set, 0 otherwise*/
int set_contains(set_t *set, void *elem)
{
    if(list_contains(set->list, elem) == 1){
        return 1;
    }
    else{
        return 0;
    }
}

/*Reverses the order of a given set*/
set_t *reverse_set(set_t *set)
{
   /*Creating new set for reversed values*/
    set_t *reversed_set = set_create(set->cmpfunc);
    if(reversed_set == NULL){
        return NULL;
    }

    /*Iterating through given set and adding elements last to reversed set*/
    set_iter_t *iterator = set_createiter(set);
    void *elem = set_next(iterator);

    while(elem != NULL){
        set_add(reversed_set, elem);
        elem = set_next(iterator);
    }
    set_destroyiter(iterator);
    set_destroy(set);

    return reversed_set;
}

/*Returns the union of the two given sets; the returned
 *set contains all elements that are contained in either a or b*/
set_t *set_union(set_t *a, set_t *b)
{
    /*Copying all elements from set A to "union set"*/
    set_t *union_set = set_copy(a);
    
    /*Iterating through set B and add all unique elements to "union set"*/
    set_iter_t *iter_b = set_createiter(b);
    void *elem_b = set_next(iter_b);

    while(elem_b != NULL){
        if(set_contains(union_set, elem_b) == 0){
            set_add(union_set, elem_b);
            elem_b = set_next(iter_b);
        }
        else{
            elem_b = set_next(iter_b);
        }
    }
    set_destroyiter(iter_b);

    /*Reversing order of "union set*/
    // union_set = reverse_set(union_set);
    // list_sort(union_set->list);

    return union_set;
}

/*Returns the intersection of the two given sets; the
 *returned set contains all elements that are contained
 *in both a and b*/
set_t *set_intersection(set_t *a, set_t *b)
{
    /*Creating new "intersection set"*/
    set_t *intersection_set = set_create(a->cmpfunc);
    if(intersection_set == NULL){
        return NULL;
    }

    /*Creating iterators for set A*/
    set_iter_t *iter_a = set_createiter(a);
    if(iter_a == NULL){
        return NULL;
    }

    /*Iterating through set A and checking if elements from it exists in set B*/
    /*Adding them to intersection set if equal values are found*/
    while(set_hasnext(iter_a) == 1){
        void *elem_a = set_next(iter_a);
        if(set_contains(b, elem_a)){
            set_add(intersection_set, elem_a);
        }
    }

    set_destroyiter(iter_a);

    /*Reversing order of the intersection set*/
    // intersection_set = reverse_set(intersection_set);
    // list_sort(intersection_set->list);
    
    return intersection_set;
}

/*Returns the set difference of the two given sets; the
 *returned set contains all elements that are contained
 *in a but not those who are equal to ones in b*/
set_t *set_difference(set_t *a, set_t *b)
{
    /*Creating new "difference set"*/
    set_t *difference_set = set_create(a->cmpfunc);
    if(difference_set == NULL){
        return NULL;
    }

    /*Creating iterator for set A*/
    set_iter_t *iter_a = set_createiter(a);
    if(iter_a == NULL){
        return NULL;
    }

    /*Iterating through set A and checking if elements from it does NOT exists in set B*/
    /*Adding them to "difference set" if NOT equal values are found*/
    while(set_hasnext(iter_a) == 1){
        void *elem_a = set_next(iter_a);
        if(set_contains(b, elem_a) == 0){
            set_add(difference_set, elem_a);
        }
    }
    set_destroyiter(iter_a);

    /*Reversing order of the difference set*/
    // difference_set = reverse_set(difference_set);
    // list_sort(difference_set->list);

    return difference_set;
}

/*Returns a copy of the given set*/
set_t *set_copy(set_t *set)
{
    /*Creating new set*/
    set_t *new_set = set_create(set->cmpfunc);
    if(new_set == NULL){
        return NULL;
    }

    /*Creating iterator for given set*/
    set_iter_t *iterator = set_createiter(set);
    if(iterator == NULL){
        set_destroy(new_set);
        return NULL;
    }

    /*Iterating through given set, and adding it's elements to new_set*/
    while(set_hasnext(iterator) != 0){
        void *elem = set_next(iterator);
        set_add(new_set, elem);
    }

    /*Destroying iterator and return new_set*/
    set_destroyiter(iterator);
    return new_set;
}

/*The type of set iterators*/
typedef struct set_iter set_iter_t;
struct set_iter{
    list_iter_t *list_iter;
};

/*Creates a new set iterator for iterating over the given set*/
set_iter_t *set_createiter(set_t *set)
{
    set_iter_t *set_iter = malloc(sizeof(set_iter_t));
    if(set_iter == NULL){
        printf("Could not create set_iterator\n");
        return NULL;
    }

    // list_sort(set->list);
    set_iter->list_iter = list_createiter(set->list);
    return set_iter;
}

/*Destroys the given set iterator*/
void set_destroyiter(set_iter_t *iter)
{
    list_destroyiter(iter->list_iter);
    free(iter);
}

/*Returns 0 if the given set iterator has reached the end of the
 *set, or 1 otherwise*/
int set_hasnext(set_iter_t *iter)
{
    return list_hasnext(iter->list_iter);
}

/*Returns the next element in the sequence represented by the given
 *set iterator*/
void *set_next(set_iter_t *iter)
{
    return list_next(iter->list_iter);
}