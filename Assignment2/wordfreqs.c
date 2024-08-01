#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "list.h"
#include "map.h"

/*Comparison function for comparing two elements
 * Returns -1 if the first is smaller than the second, 
 * Returns 1 if the first is greater than the second,
 * Returns 0 if the elements are equal.
*/
int comparestrings(void *a, void *b)
{
    return strcmp((char*)a, (char*)b);
}

/*Hashes a given key*/
//THIS IS CHANGED FROM (unsinged int) TO (unsinged long)
unsigned long elf_hash(void *ikey)
{
    unsigned int hashvalue, tmp;
    char *key = ikey;

    hashvalue = 0;

    while(*key){
        hashvalue = (hashvalue << 4) + *key;
        key++;

        if(tmp = (hashvalue & 0xf0000000)){
            hashvalue ^= tmp >> 24;
        }

        hashvalue &= ~tmp;
    }

    return hashvalue;
}

/*Creates and returns a int pointer of given value*/
int *newint(int value)
{
    int *ptr = malloc(sizeof(value));
    if(ptr == NULL){
        printf("Could not create newint\n");
        return 0;
    }

    *ptr = value;
    return ptr;
}

int main(int argc, char **argv)
{
    /*Creating map*/
    map_t *map = map_create(comparestrings, elf_hash);
    if(map == NULL){
        printf("Could not create map in main function\n");
        return 0;
    }


    /*Creating list for file words*/
    list_t *list_words = list_create(compare_strings);
    if(list_words == NULL){
        printf("Could not create list_words\n");
        return 0;
    }


    /*Creating list for unique words*/
    list_t *unique_words = list_create(compare_strings);
    if(unique_words == NULL){
        printf("Could not create unique words list\n");
        return 0;
    }


    /*Opening given files and adding their contents to list_words*/
    for(int i = 1; i < argc; i++){
        FILE *fptr = fopen(argv[i], "r");
        if(fptr == NULL){
            printf("Could not open file\n");
            return 0;
        }
        tokenize_file(fptr, list_words);
        fclose(fptr);
    }  


    /*Creating iterator for list*/
    list_iter_t *iterator = list_createiter(list_words);
    if(iterator == NULL){
        printf("Could not create iterator\n");
        return 0;
    }


    /*Adding words from list_words to map and unique_words*/
    while(list_hasnext(iterator)){
        char *word = list_next(iterator);
        if(map_haskey(map, word)){
            int *freq = map_get(map, word);
            // printf("HERE\n");
            *freq += 1;
        }
        else{
            list_addlast(unique_words, word);
            map_put(map, word, newint(1));
        }
    }
    list_destroyiter(iterator);


    /*Printing words along with their frequency*/
    iterator = list_createiter(unique_words);
    while(list_hasnext(iterator)){
        char *word = list_next(iterator);
        int *freq = map_get(map, word);
        printf("%s %d\n", word, *freq);
    }
    list_destroyiter(iterator);
    list_destroy(list_words);
    list_destroy(unique_words);
    map_destroy(map);

    return 0;
}