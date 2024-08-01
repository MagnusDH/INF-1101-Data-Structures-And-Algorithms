#include <stdio.h>
#include <stdlib.h>
#include "set.h"
#include "list.h"
#include "common.h"

#define MAXSIZE 2000


struct set{
	void **array;
	int size;
	int set_maxsize;
	cmpfunc_t cmpfunc;
};

set_t *set_create(cmpfunc_t cmpfunc)
{
	set_t *set = malloc(sizeof(set_t));						//Setter av plass til settet
	if(set == NULL){									
		return NULL;
	}
	set->size = 0;											//Størrelsen på setter er 0 til å starte med
	set->set_maxsize = MAXSIZE;								//Definerer en max størrelse
	set->cmpfunc = cmpfunc;

	set->array = malloc(sizeof(void*)*(set->set_maxsize));	//Setter av plass til arrayet	
	if(set->array == NULL)									
		return NULL;

	return set;												//Returnerer settet
}

/*
 * Destroys the given set. Subsequently accessing the set
 * will lead to undefined behavior.
 */
void set_destroy(set_t *set) 
{
	free(set->array);										//frigjør plassen som arrayet tar
	free(set);												//Frigjør plassen som settet tar
}

/*
 * Returns the size (cardinality) of the given set.
 */
int set_size(set_t *set) 
{
	return set->size; 										//returnerer settet sin størrelse
}

/*
 * Adds the given element to the given set.
 */
void set_add(set_t *set, void *elem) 
{
	set->set_maxsize = MAXSIZE;								//Definerer en max størrelse

	if (set->size >= set->set_maxsize/2){									//Hvis settet vi får tar mer plass enn halvparten av det vi har gitt
		set->set_maxsize = set->set_maxsize *2;								//Dobbler vi størrelsen på max_size
		set->array = realloc(set->array, sizeof(void*) *set->set_maxsize);	//Realokerer plass til arrayet
		if(set->array == NULL){
			return;
		}
	}

	if(set_contains(set, elem) == 0){ 						//Hvis settet som sendes inn ikke inneholder et elementet vi sender inn
		set->array[set->size] = elem;						//Så er arrayet på posisjon 0, lik det elementet vi får inn
		set->size ++;										//Størrelen på settet økes med 1
	}
}

/*
 * Returns 1 if the given element is contained in
 * the given set, 0 otherwise.
 */
int set_contains(set_t *set, void *elem)
{
	set_iter_t *iter = set_createiter(set);					//Lager en iterator for settet vi får inn

	while(set_hasnext(iter) != 0){							//Så lenge set_hasnext sier at den ikke er på enden av settet vi får inn
		void *item = set_next(iter);						//Lager en variabel som tar vare på elementet som set_next returnerer
		if(set->cmpfunc(item, elem) == 0){					//sjekk om elementet som set_next returnerer er likt med elementet vi får inn
		return 1;											//Hvis elementet er likt med elementet vi får inn returneres 1
		}
	}
	
	return 0;												//Hvis elementet er ulikt med elementet vi får inn, returneres 0
}

/*
 * Returns the union of the two given sets; the returned
 * set contains all elements that are contained in either
 * a or b.
 */
set_t *set_union(set_t *a, set_t *b)
{
	set_t *setC = set_copy(b);				//Lager et nytt sett som består av elementer fra sett b
	set_iter_t *iter = set_createiter(a);	//Lager en iterator for elementer i a
	void *elem_a = set_next(iter);			//Lager et element som itererer gjennom settet a

	while(elem_a != NULL){					//Så lenge elementet som itererer i settet a ikke er på NULL
		if(set_contains(setC, elem_a) == 0){//Sjekk om element i setC/b er ulikt med a
			set_add(setC, elem_a);			//Hvis de er ulik, legges elementet inn i setC
			elem_a = set_next(iter);		//Itererer til neste element i a for videre sammenligning
		}
		else {								//Hvis element i a er likt med element i b
			elem_a = set_next(iter);		//Itererer til neste element i a
		}
	}
	set_destroyiter(iter);					//Sletter iteratoren
	return setC;							//Returnerer et sett med elementer fra a og b
}

/*
 * Returns the intersection of the two given sets; the
 * returned set contains all elements that are contained
 * in both a and b.
 *///Sjekker om verdier i set_a og set_b er lik, hvis de er lik addes de til et nytt sett
set_t *set_intersection(set_t *a, set_t *b)
{
	set_t *setC = set_create(a->cmpfunc);	//Lager et sett som skal inneholde alle like elementer fra a og b
	set_iter_t *iter_a = set_createiter(a); //lager en iterator for elementer i a
	void *elem_a = set_next(iter_a); 		//Lager en variabel som itererer gjennom set a
	
	while(elem_a != NULL){ 					//så lenge det er elementer i a
		if(set_contains(b, elem_a) == 1){ 	//sjekk om elementer i b er likt med elementer i a
			set_add(setC, elem_a); 			//Hvis de er lik, legg de til i setC
			elem_a = set_next(iter_a); 		//Itererer til neste element i a for sammenligning
		}
		else { 								//Hvis element i a er ulikt med et element i b
			elem_a = set_next(iter_a); 		//Iterer til neste element
		}
	}
	set_destroyiter(iter_a);				//Ødlegg iteratoren

	return setC;							//Returner setC
}

/*
 * Returns the set difference of the two given sets; the
 * returned set contains all elements that are contained
 * in a and not in b.
 */
set_t *set_difference(set_t *a, set_t *b)
{
	set_t *setC = set_create(setC->cmpfunc);	//Lager et sett som skal inneholde elementer som er i a, men ikke de som er lik med b
	set_iter_t *iter_a = set_createiter(a);		//Lager en iterator som itererer gjennom elementer i a
	void *elem_a = set_next(iter_a);			//En variabel som itererer gjennom set a

	while(elem_a != NULL){						//Så lenge det er elementer i a
		if(set_contains(b, elem_a) == 0) {		//Hvis element a er ulikt med det i b
			set_add(setC, elem_a);				//Legg elementet til setC
			elem_a = set_next(iter_a);			//Iterer til neste element
		}
		else {									//Hvis element i a er likt med et element i b
			elem_a = set_next(iter_a);			//Itterer til neste element i a
		}
	}
	set_destroyiter(iter_a);					//Ødelegg iteratoren
	
	return setC;								//Returner setC
}

/*
 * Returns a copy of the given set.
 */
set_t *set_copy(set_t *set)
{
	set_t *copyset = set_create(set->cmpfunc);	//Lager et nytt sett som skal inneholde elementene i et kopiert sett
	if(copyset == NULL)							//Sjekker om settet er tomt
		return NULL; 							//Hvis det er tomt returneres NULL
	

	set_iter_t *iter_copy = set_createiter(set);//Lager en iterator som itererer gjennom et gitt sett
	if(iter_copy == NULL){						//Sjekker om iteratoren er tom
		set_destroy(copyset);					//Slett settet
		return NULL; 							//Hvis den er tom returneres NULL
	}

	while(set_hasnext(iter_copy) != 0){ 		//Sålenge iteratoren ikke er på slutten av settet
		void *item = set_next(iter_copy);		//Lager en variabel som returnerer nåværende element og itererer til neste element i et gitt sett
		set_add(copyset, item); 				//Legger elementet inn i det nye settet
	}
	set_destroyiter(iter_copy); 				//Ødlegger iteratoren
	
	return copyset; 							//Returnerer det kopierte settet
}

/*
 * Sorts the elements of the given set, using the comparison function
 * of the set to determine the ordering of the elements.
 */
static void set_selection_sort(set_t *set)
{
	int min, i, j;
	

	if(set->size < 2){												//Hvis settet består av 1 element er det ikke noe å sortere, og funksjonen stopper
		return;
	}

	for(i = 0; i < set->size; i++){									//kjører loop sålenge det er elementer i arrayet og itererer til neste element i arrayet
		min = i;													//min er lik første spalte av arrayet
		for(j = i+1; j < set->size; j++){							//Kjører en ny loop så lenge det er elementer i arrayet og iterer til neste element i arrayet
			if(set->cmpfunc(set->array[j], set->array[min]) < 0){	//Sjekker om elementet i settet på nåværende posisjon minus et element i settet er mindre enn 0. Sjekker da om hvilket element/tall som er størst eller om de er lik
				min = j;											//Hvis "j" minus "min" er mindre enn 0, så er "min" lik neste element i settet, da "j" inkrementeres i loopen  
			}	
		}

		if(min != i){												//Hvis "min" er ulik med "i"
			void *tmp = set->array[min];							//Lages en tmp variabel som tar vare på verdien til "min"
			set->array[min] = set->array[i];						//Setter det andre elementet til å være "i" sin posisjon i arrayet
			set->array[i] = tmp; 									//Setter "i" sin posisjon til å være "tmp" sin posisjon, (basicly bytter om på plasseringene)
		}
	}
}
/*
 * The type of set iterators.
 */
//Lager en set iterator
struct set_iter
{
	set_t *set;
	int index;
};


/*
 * Creates a new set iterator for iterating over the given set.
 */
//funksjonen setter av plass til en sett type (sett_iter_t)
set_iter_t *set_createiter(set_t *set)
{
	set_iter_t *set_iter = malloc(sizeof(set_iter_t));	//Setter av plass til iteratoren 
	if(set_iter == NULL){								
		return NULL;									
	}

	set_selection_sort(set);

	set_iter->index = 0;
	set_iter->set = set;

	return set_iter;
}

/*
 * Destroys the given set iterator.
 */
//Sletter sett-iteratoren ved hjelp av list_destroyiter funksjonen og frigir iteratoren
void set_destroyiter(set_iter_t *iter)
{
	free(iter);
}

/*
 * Returns 0 if the given set iterator has reached the end of the
 * set, or 1 otherwise.
 */ 
int set_hasnext(set_iter_t *iter)
{
	if (iter->index >= iter->set->size){
		return 0;
	}
	else {
		return 1;
	}
}

	//iter->set->array[iter->index+1] == NULL
/*
 * Returns the next element in the sequence represented by the given
 * set iterator.
 */
void *set_next(set_iter_t *iter)
{
	if(iter->index >= iter->set->size){	//Hvis elementet er NULL
		return NULL;	//Returneres NULL
	}
	else {				//Hvis elementet ikke er NULL
		void *elem = iter->set->array[iter->index];	// 
		iter->index++;
		
		return elem;
	}
}
