#include <stdio.h>
#include <stdlib.h>
#include <math.h>
  
#include "index.h"
#include "list.h"
#include "map.h"
#include "parser.h"
#include "set.h"
#include "common.h"
/*
typedef struct query_result
{
	char *path;    
	double score;  
}query_result_t;
*/

/*
 * Lager en ny, tom index
 */
index_t *index_create()
{
	index_t *index = malloc(sizeof(index_t));				//Setter av plass til indexen
	if (index == NULL){										//Hvis indexen er tom
		return NULL;										//returner NULL
	}

	index->map = map_create(compare_strings, hash_string);	//Tar vare på map'en til index structuren
	
	return index;											//Returnerer indexen
}

/*
 Funksjonen
*/
query_result_t *query_create()
{
	query_result_t *new = malloc(sizeof(query_result_t));	//Allokerer plass til query_result typen
	if (new == NULL){
		return NULL;
	}

	new->path = NULL;
	new->score = 0;

	return new;
}

/*
 *Sletter et gitt set
 */
void destroy_value(void *set)
{
	set_destroy(set);
}

/*
 * Ødlegger indexen
 * Subsequently accessing the index will lead to undefined behavior.
 */
void index_destroy(index_t *index)
{
	map_destroy(index->map, free, destroy_value);
	free(index);
}
/*
Teller hvor mange ganger et ord forekommer i en liste 
*/
double counter(list_t *list, char *word)
{
	double counter = 0;
	list_iter_t *iter = list_createiter(list);

	while (list_hasnext(iter) != 0){
		char *elem = list_next(iter);
		if (compare_strings(word, elem) == 0){
			counter ++;
		} 
	}
	return counter;
}

/*
 * Adds the given path to the given index, and index the given
 * list of words under that path.
 * NOTE: It is the responsibility of index_addpath() to deallocate (free)
 *       'path' and the contents of the 'words' list.
 * 
 * Funksjonen itererer gjennom en gitt liste med ord fra et spesifikt dokument (list_t *words, git fra indexer.c) og sjekker om ordene finnes i et map
 * Finnes ordet i map'en, blir map'en hentet ut og settet med dokumenter tilhørende ordet blir lagt til i map'et
 * Finner IKKE ordet i map'en blir et nytt set laget, puttet inn i map'en og dokumenter tilhørende ordet blir lagt til.
*/

void index_addpath(index_t *index, char *path, list_t *words)
{
	int wordcounter;
	list_iter_t *list_iter = list_createiter(words);					//Lager en liste-iterator for listen words vi får som argument

	while(list_hasnext(list_iter)){										//Så lenge det er ord i listen words
			void *word = list_next(list_iter);							//Returneres et ord fra nåværende posisjon			
			query_result_t *result = query_create();					//Lager en query_result type

			if (map_haskey(index->map, word) == 1){						//Hvis index->map(selve mappet) inneholder dette ordet/nøkkelen
				set_t *set = map_get(index->map, word);					//Henter jeg ut denne map'en med dens value, som er et sett med dokumenter
				if (set_contains(set, path) == 0){						//Hvis settet med dokumenter IKKE inneholder mitt spesifike dokument (path)
					result->path = strdup(path);						//Legger dokumentene i query_result sin dokument-kilde(path)
					set_add(set, result);								//Adder query_result dokumentene/path'en til settet	
				} else {												//Hvis settet med dokumenter inneholder mitt spesifikke dokument (path)	
					break;												//Ikke gjør noe
				}
			}
			else {														//Hvis index->map IKKE inneholder ordet på nåværende posisjon
				set_t *secondset = set_create(compare_strings);			//Lager jeg et nytt set(secondset)
				result->path = strdup(path);							//Legger dokumentene i query_result sin dokument-kilde(path)
				set_add(secondset, result);								//Og legger til query_result dokumentene/path'en til secondset
				map_put(index->map, strdup(word), secondset);			//Putter dette settet(secondset) inn i map'en
			}
//TF-IDF: (antall ganger et ord forekommer i ETT dokument) * log(totalt antall dokumenter(3204) / hvor mange dokumenter som inneholder ordet (set_size))
			wordcounter = counter(words, word);							//Teller hvor mange ganger et spesifikt ord forekommer i ett gitt dokument
			set_t *map_set = map_get(index->map, word);					//Henter ut settet med nøkkelen word fra mappet 
			result->score = wordcounter * log(3204/set_size(map_set));	//Regner ut TF_IDF resultatet

			//result->score = (wordcounter/list_size(words)) * log(3204/set_size(map_set));	//Regner ut TF_IDF resultatet
			
			free(word);
	}
	free(path);
	list_destroyiter(list_iter);										//Ødlegger iteratoren når jeg er ferdig med den
}


/*
 * Performs the given query on the given index. If the query
 * succeeds, the return value will be a list of paths (query_result_t). 
 * If there is an error (e.g. a syntax error in the query), an error 
 * message is assigned to the given errmsg pointer and the return value
 * will be NULL.
 * 
 * Funksjonen får inn en liste med ord, prossesert fra indexer.c. Disse ordene er query som er skrevet inn i web-browseren.
 * index_query skal så prosessere denne listen og returnere en ny liste med dokumenter som hører til ordene som ble presentert
 */
list_t *index_query(index_t *index, list_t *query, char **errmsg)
{
	parsenode_t *parsetree = parse(query);			//Starter å parse (query) listen som vi får inn, får returnert et parse-tre
	
	list_t *list = list_create(compare_strings);	//Lager en liste som skal bli returnert mot slutten av funksjonen

	set_t *set = evaluate(parsetree, index);		//Evaluerer parse-treet og returnerer et set med dokumenter
	set_iter_t *set_iter = set_createiter(set);		//Lager en iterator for settet med dokumenter

	while (set_hasnext(set_iter) != 0){				//Så lenge det er elementer i settet
		void *elem = set_next(set_iter);			//Henter ut et element fra settet
		list_addfirst(list, elem);					//Adder elementet fra set til listen
	}

	set_destroyiter(set_iter);						//Ødlegger iteratoren
	
	return list;									//Returnerer en liste med dokumenter
}