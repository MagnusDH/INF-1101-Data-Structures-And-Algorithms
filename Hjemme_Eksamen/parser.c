#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "common.h"
#include "map.h"
#include "list.h"
#include "set.h"
#include "index.h"

/*
 Funksjon som lager en ny node til syntaks treet
 */
static parsenode_t *parse_query(list_t *query);
static parsenode_t *parse_andterm(list_t *query);
static parsenode_t *parse_orterm(list_t *query);
static parsenode_t *parse_term(list_t *query);


/*
Funksjon som lager en ny node til syntaks treet
*/
static parsenode_t *newnode(node_type type, char *word, parsenode_t *left, parsenode_t *right)
{
    parsenode_t *node = malloc(sizeof(parsenode_t)); //Setter av plass til noden
    if (node == NULL){
        fatal_error("Fritt for minne");
    }

    node->type = type;
    node->word = word;
    node->left = left;
    node->right = right;

    return node;
}


/*
Funksjon som starter parsingen av en gitt liste og
returnerer en som erklærer starten av et tre
*/
parsenode_t *parse(list_t *query)
{
    parsenode_t *node;                              //Definerer en node type
    list_iter_t *iter = list_createiter(query);     //Lager en listeiterator for listen (query)

    if (list_hasnext(iter) != 0){                   //Hvis det er elementer i listen vi får inn
        node = parse_query(query);                  //Starter parserprosessen på listen vi får
        return node;                                //Returnerer noden som er starten av parsetreet.
    }
    else {
        fatal_error("No query given");              //Ingen ord i listen
        return NULL;
    }
}


/*
   query   ::= andterm
        | andterm "ANDNOT" query 
    
*Query kan være et andterm, men det kan også være et andterm etterfulgt av ANDNOT og et nytt query

* Funkjsonen deler opp et gitt input, sjekker for et "andterm" og til slutt sjekker for en ANDNOT operatør
* Om den finner en ANDNOT operatør returneres en node av typen "ANDNOT" med p1 og p2 som barn
* Ble ingenting funnet etter alle prosessene som delene ble sendt igjennom, returneres bare ordet aka p1
*/
static parsenode_t *parse_query(list_t *query)
{
    parsenode_t *p1 = parse_andterm(query);                             //Sender query listen videre til andterm
    parsenode_t *p2;                                                    //Definerer en parsenode type
    list_iter_t *iter = list_createiter(query);                         //Lager en iterator for listen (query)

    if (list_hasnext(iter) != 0){                                       //Så lenge det er elementer i listen
        void *tmp = list_next(iter);                                    //Returneres første element
        
        if ((strcmp(tmp, "ANDNOT") == 0) || (strcmp(tmp, "andnot") == 0)){  //Hvis adressen til nåværende ord er lik strengen "ANDNOT"
            void *elem = list_popfirst(query);                          //Fjerner vi første ordet fra listen (query)
            list_destroyiter(iter);                                     //Ødlegger iteratoren
            p2 = parse_query(query);                                    //Kjører rekursiv parse_query
            return newnode(ANDNOT, elem, p1, p2);                       //Dersom ANDNOT ble funnet returneres en ny node av typen ANDNOT med ordet som ble funnet med p1 og p2 som barn
        }
    }
    return p1;                                                          //Ble ikke ANDNOT operatøren funnet returneres bare ordet
}


/*
andterm ::= orterm

        | orterm "AND" andterm

* andterm kan være et orterm, men det kan også være et orterm etterfulgt av operatøren AND og et nytt andterm

* Funksjonen deler opp et gitt input, sender delene videre til "orterm" og til slutt sjekker for en AND operatør
* Om den finner en AND operatør returneres en node av typen AND med p1 og p2 som barn
* Blir ingenting funnet etter alle prosessene som delene ble videresendt til, returneres bare ordet aka p1
*/
static parsenode_t *parse_andterm(list_t *query)
{
    parsenode_t *p1 = parse_orterm(query);                          //Sender query listen videre til orterm
    parsenode_t *p2;                                                //Definerer en parsenode type
    list_iter_t *iter = list_createiter(query);                     //Lager en iterator for listen (query)

    if (list_hasnext(iter) != 0){                                   //Så lenge det er elementer i listen
        void *tmp = list_next(iter);                                //Returneres første element i listen

        if ((strcmp(tmp, "AND") == 0) || (strcmp(tmp, "and") == 0)){    //Hvis nåværende ord er lik strengen "AND"
            void *elem = list_popfirst(query);                                   //Fjernes og returneres første ord i listen (query)
            list_destroyiter(iter);                                 //Iteratoren ødlegges
            p2 = parse_andterm(query);                              //Kjører rekursiv andterm pånytt for å sjekke om AND forekommer i neste del av input
            return newnode(AND, elem, p1, p2);                      //Dersom AND ble funnet returneres en ny node av typen AND og ordet som ble funnet med p1 og p2 som barn
        }
    }
    return p1;                                                      //Ble ikke AND operatøren funnet returneres bare ordet
}


/*
orterm  ::= term
        | term "OR" orterm

*orterm kan være et term, med det kan også være et term etterfulgt av en OR operatør og et nytt orterm

*Funksjonen sender del1 og del2 videre til parse_term og til slutt sjekker for OR operatøren
*Om den finner en OR operatør returneres en ny node av typen OR med p1 og p2 som barn
*Blir ingenting funnet etter de resterende prosessene returneres bare ordet
*/
static parsenode_t *parse_orterm(list_t *query)
{
    parsenode_t *p1 = parse_term(query);                            //Sender query listen videre for å hente ut et ord
    parsenode_t *p2;                                                //Definerer en parsenode type
    list_iter_t *iter = list_createiter(query);                     //Lager en iterator for listen (query)

    if (list_hasnext(iter) != 0){                                   //Så lenge det er elementer i listen (query)
        void *tmp = list_next(iter);                                //Returneres første element i listen

        if ((strcmp(tmp, "OR") == 0) || (strcmp(tmp, "or") == 0)){      //Hvis nåværende element er lik strengen "OR" 
            void *elem = list_popfirst(query);                      //Fjernes og returneres første ord i listen (query)
            list_destroyiter(iter);                                 //Iteratoren ødlegges
            p2 = parse_orterm(query);                               //Kjører rekursiv orterm pånytt på resterende av strengen for å sjekke om OR forekommer som neste ord
            return newnode(OR, elem, p1, p2);                       //Dersom OR ble funnet returneres en ny node av typen OR og ordet som ble funnet med p1 og p2 som barn
        }
    }
    return p1;                                                      //Ble ikke OR operatøren funnet returneres resterende streng
}


/*
term    ::= "(" query ")"
        | <word>

*term kan være et nytt query, men det kan også bare være ett ord
*Funksjonen analyserer en streng og returnerer en node med ordet som blir analysert
*/
static parsenode_t *parse_term(list_t *query)
{
    void *elem = list_popfirst(query);                  //Fjerner og returneres første element i listen (query)

    return newnode(WORD, elem, NULL, NULL);             //Returnerer ny node av typen WORD med ordet som ble fjernet fra listen med NULL og NULL som barn
}


/*
Funksjon for sammenligning av query_result typer
*/
int compare_query_path(void *a, void *b)                
{
    query_result_t *tmp = a, *tmp2 = b;
    return (strcmp(tmp->path, tmp2->path));
}


/*
 Funksjonen evaluerer nodene i parse-treet og returnerer et set avhengig av hvilke noder som blir funnet
 */
set_t *evaluate(parsenode_t *node, index_t *index)
{
    switch(node->type){                                                                         //Bytter mellom de forskjellige node-typene
        case WORD:                                                                              //I tilfellet en WORD node er funnet
            if (map_haskey(index->map, node->word) == 1){                                       //Hvis map inneholder node-ordet
                set_t *newset = set_create(compare_query_path);                                 //Lager et nytt sett
                set_t *tmp = map_get(index->map, node->word);                                   //Henter ut map'et med ordet
                set_iter_t *iter = set_createiter(tmp);                                         //Lager en iterator for tmp settet

                while (set_hasnext(iter) != 0){                                                 //Så lenge det er elementer i tmp-settet
                    query_result_t *elem = set_next(iter);                                      //Henter ut første element
                    query_result_t *copy = query_create();                                      //Lager en ny type som skal være en kopi for å ikke rote med innholdet i det originale mappet
                    copy->path = elem->path;                                                    //Setter kopien til å være elem
                    copy->score = elem->score;                                                  //Setter kopien til å være elem
                    set_add(newset, copy);                                                      //Legger til kopien i et nytt sett
                }
                return newset;                                                                  //Returnerer det nye settet
            }
            else {                                                                              //Hvis map IKKE inneholder node-ordet
                set_t *emptyset = set_create(compare_query_path);                               //Lager et nytt set som betyr at ingen dokumenter inneholder ordet
                return emptyset;                                                                //Returnerer et tomt sett som betyr at et ord ikke finnes i et dokument
            }
            break;
        case AND:                                                                               //I tilfellet en AND operatør er funnet
            return set_intersection(evaluate(node->left, index), evaluate(node->right, index)); //Returnerer set med de dokumenter som inneholder begge søkeord
        case OR:                                                                                //I tilfellet en OR operatør er funnet
			return set_union(evaluate(node->left, index), evaluate(node->right, index));        //Returnerer set med alle dokumenter som inneholder begge søkeord 
		case ANDNOT:                                                                            //I tilfellet en ANDNOT operatør er funnet 
			return set_difference(evaluate(node->left, index), evaluate(node->right, index));   //Returnerer set med dokumenter som kun inneholder første søkeord
    }

    set_t *emptyset = set_create(compare_strings);                                              //Hvis en node-type som IKKE er gjenkjent lages et tomt set
    return emptyset;                                                                            //Det tomme settet returneres
}