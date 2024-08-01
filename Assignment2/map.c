#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "map.h"
#include "list.h"

typedef struct map map_t;
struct map{
    hashfunc_t hashfunc;
    cmpfunc_t cmpfunc;
    int current_size;
    int max_size;
    list_t **list;
};

typedef struct map_item map_item_t;
struct map_item{
    void *key;              //Key to where item is in map
    void *value;            //Actual value stored
    unsigned int hashvalue; //Hashvalue for this particular item
};

/*Creates a new, empty map*/
map_t *map_create(cmpfunc_t cmpfunc, hashfunc_t hashfunc)
{
    map_t *map = malloc(sizeof(map_t));
    if(map == NULL){
        printf("Could not create map in map_create function\n");
        return NULL;
    }

    map->hashfunc = hashfunc;
    map->cmpfunc = cmpfunc;
    map->current_size = 0;
    map->max_size = 17;
    
    map->list = calloc(map->max_size, sizeof(list_t *));
    if(map->list == NULL){
        printf("Could not calloc for map->list. See map_create function\n");
        return NULL;
    }

    return map;
}

/*Destroys the given map and everything in it*/
void map_destroy(map_t *map)
{
    int i = 0;
    while(i < map->max_size){
        if(map->list[i] != NULL){
            list_destroy(map->list[i]);
        }
        i++;
    }

    free(map);
}

/*Resizes the map to double its max_size*/
int map_resize(map_t *map)
{
    int oldsize = map->max_size;
    int newsize = map->max_size*2;

    list_t **old_list = map->list;
    /*Creating new list of new size*/
    list_t **new_list = calloc(newsize, sizeof(list_t *));
    if(new_list == NULL){
        printf("Could not resize map, see map_resize function\n");
        return 0;
    }

    /*Updating list to new variables*/
    map->list = new_list;
    map->max_size = newsize;
    map->current_size = 0;

    /*Going through all items in list, and replace them to correct indexes*/
    for(int i = 0; i<oldsize; i++){
        if(old_list[i] != NULL){
            map_item_t *tmp = list_popfirst(old_list[i]);
            while(tmp != NULL){
                map_put(map, tmp->key, tmp->value);
                tmp = list_popfirst(old_list[i]);
            }
            list_destroy(old_list[i]);
        }
    }
    free(old_list);
    return 1;
}

/*Puts a value in the map, and associates it with a given key
 *This will overwrite any value previously located on the same key
*/
int map_put(map_t *map, void *key, void *value)
{
    /*Checking if given key already exists in map*/
    if(map_get(map, key) != NULL){
        printf("Key already exists in map\n");
        return 0;
    }

    /*Resizing map if it gets bigger than half of max_size*/
    if(map->current_size >= (map->max_size/2)){
        if(map_resize(map) == 0){
            printf("Could not resize map, see map_put function\n");
            return 0;
        }
    }

    /*Finding at which hashvalue/key/index the value should be placed at*/
    unsigned long hashvalue = map->hashfunc(key);
    /*Calculating index for where to put value in map*/
    int index = hashvalue % map->max_size;

    /*Creating list on index in map, if there is no list there already*/
    if(map->list[index] == NULL){
        map->list[index] = list_create(map->cmpfunc);
        if(map->list[index] == NULL){
            printf("Could not create list at index %d in map, see map_put function\n", index);
            return 0;
        }
    }

    /*Creating item to put in list*/
    map_item_t *item = malloc(sizeof(map_item_t));
    if(item == NULL){
        printf("Could not create item to put in list, see map_put function\n");
        free(item);
        return 0;
    }

    item->key = key;
    item->value = value;
    item->hashvalue = hashvalue;

    /*Adding item to list at map-index*/
    list_addlast(map->list[index], item);
    
    /*Increasing map's size with 1*/
    map->current_size++;

    return 1;
}

/*Returns 1 if the given map contains the given key, 0 otherwise*/
int map_haskey(map_t *map, void *key)
{
   /*Hashing given key to find a map-index to look for value*/
   unsigned long hashvalue = map->hashfunc(key);
   int index = hashvalue % map->max_size;
   
   /*Checking if there is a list on index in map*/
   if(map->list[index] != NULL){
       return 1;
   }

   return 0;
}

/*Returns the value the given key is associated with*/
void *map_get(map_t *map, void *key)
{
    /*Hashing given key to find map-index to look for value*/
    unsigned long hashvalue = map->hashfunc(key);
    int index = hashvalue % map->max_size;

    /*Checking if there is a list on this index*/
    if(map->list[index] == NULL){
        // printf("Could not get value from map, there is no list at index %d. See map_get function\n", index);
        return NULL;
    }

    /*Creating iterator for list*/
    list_iter_t *iterator = list_createiter(map->list[index]);
    if(iterator == NULL){
        printf("Could not create iterator for list, see map_put function\n");
        return NULL;
    }

    /*Going through list to find requested value*/
    map_item_t *current = list_next(iterator);
    while(current != NULL){
        if(current->hashvalue == hashvalue && map->cmpfunc(key, current->key) == 0){
            break;
        }
        else{
            current = list_next(iterator);
        }
    }
    list_destroyiter(iterator);


    /*Checking if value has been found*/
    if(current != NULL){
        return current->value;
    }
    else{
        return NULL;
    }
}