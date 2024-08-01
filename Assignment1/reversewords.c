#include "list.h"
#include "common.h"

/*Comparison function for comparing two elements
 * Returns -1 if the first is smaller than the second, 
 * Returns 1 if the first is greater than the second,
 * Returns 0 if the elements are equal.
 * */
int comparefunction(void *a, void *b)
{
    int ret = strcmp((char *)a, (char *)b);

    //If (a-b) < 0
    if(ret > 0){
        // printf("first is lowest\n");
        return -1;
    }
    //If (a-b) == 0
    if(ret == 0){
        // printf("Numbers are equal\n");
        return 0;
    }
    //If (a-b) > 0
    if(ret < 0){
        // printf("second is lowest\n");
        return 1;
    }
}

int main(int argc, char **argv)
{
    /*Creating list*/
    list_t *list = list_create((cmpfunc_t) comparefunction);
    if(list == NULL){
        printf("Could not create list\n");
        return 0;
    }

    /*
    Opening given file and adding it's
    contents to list
    */
    for(int i = 1; i < argc; i++){
        FILE *fptr = fopen(argv[i], "r");
        if(fptr == NULL){
            printf("Could not open file\n");
            return 0;
        }
        tokenize_file(fptr, list);
        fclose(fptr);
    }  


    /*Creating iterator for list*/
    list_iter_t *iterator = list_createiter(list);
    if(iterator == NULL){
        printf("Could not create iterator\n");
        return 0;
    }

    /*Printing contents of list*/
    printf("Original list:\n");
    while(list_hasnext(iterator)){
        char *word = list_next(iterator);
        printf("%s->", word);
    }
    printf("\n");


    /*Printing reversed list*/
    printf("Printing reversed list\n");
    while (list_size(list) > 1) {
	    char *word = list_poplast(list);
	    printf("%s->", word);
    }
    printf("\n");
    list_destroy(list);

    return 0;
}