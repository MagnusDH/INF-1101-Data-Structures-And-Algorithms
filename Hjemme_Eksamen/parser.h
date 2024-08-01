#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "set.h"
#include "index.h"


typedef enum {  //Enumerasjon der man kan lage variabelnavn slik at det er lettere å lese koden
    AND, OR, ANDNOT, WORD
} node_type;    //Kaller dette for node_type

typedef struct parsenode parsenode_t;
struct parsenode {
    char *word;                             //Noden trenger å inneholde en verdi
    node_type type;                         //En av verdiene: AND, OR, ANDNOT
    parsenode_t *left, *right;              //må inneholde en høyre og venstre gren
};


/*
 Funksjon som starter parsingen av en gitt liste og
 returnerer et tre med noder
 */
//static parsenode_t *parse(list_t *query);

parsenode_t *parse(list_t *query);

/*
 * Funkjsonen deler opp et gitt input, sjekker for et "andterm" og til slutt sjekker for en ANDNOT operatør
 * Om den finner en ANDNOT operatør returneres en node av typen "ANDNOT" med p1 og p2 som barn
 * Ble ingenting funnet etter alle prosessene som delene ble sendt igjennom, returneres bare ordet aka p1
 */
//static parsenode_t *parse_query(list_t *query);

set_t *evaluate(parsenode_t *node, index_t *index);

#endif