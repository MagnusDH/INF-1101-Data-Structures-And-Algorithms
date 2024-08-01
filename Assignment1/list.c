#include "list.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct node node_t;
struct node{
    node_t *next;
    node_t *previous;
    void *item; 
};

typedef struct list list_t;
struct list{
    node_t *head;
    node_t *tail;
    int size;
    cmpfunc_t cmpfunc;
};


/*
 * Creates a new, empty list that uses the given comparison function
 * to compare elements.  The comparison function accepts two elements,
 * and returns -1 if the first is smaller than the second, 1 if the
 * first is greater than the second, and 0 if the elements are equal.
 *
 * Returns the new list.
 */
list_t *list_create(cmpfunc_t cmpfunc)
{
    list_t *list = malloc(sizeof(list_t));
    if(list == NULL){
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->cmpfunc = cmpfunc;

    return list;
}

/*
 * Destroys the given list and every node in it
 */
void list_destroy(list_t *list)
{
     node_t *tmp = list->head;

    while(list->head != NULL){
        tmp = list->head->next;
        free(list->head);
        list->head = tmp;    
    }
    
    free(list);
}

/*
 * Returns the current size of the given list.
 */
int list_size(list_t *list)
{
   return list->size;
}

/*
 * Adds the given element to the start of the given list.
 * Returns 1 on success, and 0 if the operation failed.
 */
int list_addfirst(list_t *list, void *elem)
{
    node_t *new_node = malloc(sizeof(node_t));
    if(new_node == NULL){
        return 0;
    }

    new_node->item = elem;

    if(list->head == NULL){
        list->head = new_node;
        list->tail = new_node;
    }
    else{
        list->head->previous = new_node;
        new_node->next = list->head;
        list->head = new_node;
    }

    list->size++;
    return 1;
}

/*
 * Adds the given element to the end of the given list.
 * Returns 1 on success, and 0 if the operation failed.
 */
int list_addlast(list_t *list, void *elem)
{
    /*Checking if given list is empty*/
    if(list->head == NULL){
        list_addfirst(list, elem);
    }
    else{
        node_t *node = malloc(sizeof(node_t));
        if(node == NULL){
            return 0;
        }
        node->item = elem;
        list->tail->next = node;
        node->previous = list->tail;
        list->tail = node;
    }

    list->size++;
    return 1;
}

/*
 * Removes and returns the first element of the given list.
 */
void *list_popfirst(list_t *list)
{
    if(list->head == NULL){
        printf("Cannot popfirst from list, it's empty\n");
        return NULL;
    }
    else{
        void *item = list->head->item;
        node_t *tmp = list->head;
        list->head = list->head->next;
        
        /*Checking if list now is empty*/
        if(list->head == NULL){
            list->tail = NULL;
        }
        else{
            list->head->previous = NULL;
        }

        list->size--;
        free(tmp);
        return item;
    }
}

/*
 * Removes and returns the last element of the given list.
 */
void *list_poplast(list_t *list)
{
    if(list->head == NULL){
        printf("Could not pop last element, list is empty\n");
        return NULL;
    }
    else{
        void *item = list->tail->item;
        node_t *tmp = list->tail;
        list->tail = list->tail->previous;

        /*Checking if list now is empty*/
        if(list->tail == NULL){
            list->head = NULL;
        }
        else{
            list->tail->next = NULL;
        }

        free(tmp);
        list->size--;
        return item;
    }
}

/*
 * Returns 1 if the given list contains the given element, 0 otherwise.
 *
 * The comparison function of the list is used to check elements for equality.
 */
int list_contains(list_t *list, void *elem)
{
    if(list->head == NULL){
        printf("List does not contain anything, it's empty\n");
        return 0;
    }
    else{
        node_t *node = list->head;
        while(node != NULL){
            if(list->cmpfunc(elem, node->item) == 0){
                return 1;
            }
            node = node->next;
        }
    }
    return 0;
}

/*
 * Sorts the elements of the given list, using the comparison function
 * of the list to determine the ordering of the elements.
 */
void list_sort(list_t *list)
{
    if(list->size < 2){
        printf("Can not sort list, only 1 element in it\n");
        return;
    }

    node_t *lowest, *i, *j;

    /*This loop sorts in correct order (1,2,3... etc)*/
    for(i = list->tail; i != NULL; i = i->previous){
        lowest = i;
        for(j = i->previous; j != NULL; j = j->previous){
            if(list->cmpfunc(j->item, lowest->item) < 0){
                lowest = j;
            }
        }
        if(lowest != i){
            void *tmp = lowest->item;
            lowest->item = i->item;
            i->item = tmp;
        }
    }
    
    /*This loop sorts in reversed order (9,8,7...etc)*/
    // for(i = list->head; i != NULL; i = i->next){
    //     lowest = i;
    //     for(j = i->next; j != NULL; j = j->next){
    //         if(list->cmpfunc(j->item, lowest->item) < 0){
    //             lowest = j;
    //         }
    //     }
    //     if(lowest != i){
    //         void *tmp = lowest->item;
    //         lowest->item = i->item;
    //         i->item = tmp;
    //     }
    // }
}   

/*
 * The type of list iterators.
 */
typedef struct list_iter list_iter_t;
struct list_iter{
    node_t *current;
    list_t *list;
};

/*
 * Creates a new list iterator for iterating over the given list.
 */
list_iter_t *list_createiter(list_t *list)
{
    list_iter_t *iterator = malloc(sizeof(list_iter_t));
    if(iterator == NULL){
        return NULL;
    }

    iterator->current = list->head;
    iterator->list = list;

    return iterator;
}

/*
 * Destroys the given list iterator.
 */
void list_destroyiter(list_iter_t *iter)
{
    free(iter);
}

/*
 * Returns 0 if the given list iterator has reached the end of the
 * list, or 1 otherwise.
 */
int list_hasnext(list_iter_t *iter)
{
    if(iter->current == NULL){
        return 0;
    }
    else{
        return 1;
    }
}

/*
 * Returns the next element in the sequence represented by the given
 * list iterator.
 */
void *list_next(list_iter_t *iter)
{
    if(iter->current == NULL){
        return NULL;
    }
    
    void *item = iter->current->item;
    iter->current = iter->current->next;

    return item;
}

/*
*Resets the iterator to list->head
*/
void reset_iterator(list_iter_t *iterator)
{
    iterator->current = iterator->list->head;
}