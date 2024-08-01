#include <stdio.h>
#include <stdlib.h>
#include "set.h"
#include "list.h"
#include "common.h"
#include <sys/time.h>


struct set{ //Struktur/datatype for "set"som kan inneholde mange typer variabler
	list_t *list;
	int size;
	cmpfunc_t cmpfunc;
};

set_t *set_create(cmpfunc_t cmpfunc)
{
	set_t *set = malloc(sizeof(set_t)); //Setter av plass til settet
	if (set == NULL)					//hvis settet er tomt
		return NULL;					//returner null

	set->list = list_create(cmpfunc); 	//Lager et sett ved hjelp av list create
	set->size = 0;						//Størrelsen til settet
	set->cmpfunc = cmpfunc;				//Tar vare på cmpfunc til videre bruk

	return set;
}
/*
 * Destroys the given set. Subsequently accessing the set
 * will lead to undefined behavior.
 */
void set_destroy(set_t *set) 
{
	list_destroy(set->list); //bruker list destroy som er laget for oss, og bruker den via settet
	free(set); //tømmer lista siden dette er implementert utenfor funksjonen
}

/*
 * Returns the size (cardinality) of the given set.
 */
int set_size(set_t *set) 
{
	return set->size; //returnerer settet sin størrelse
}

/*
 * Adds the given element to the given set.
 */
void set_add(set_t *set, void *elem) 
{
	if(set_contains(set, elem) == 0){		//Hvis set ikke inneholder et element som allerede finnes
		list_addfirst(set->list, elem);		//sett elementet inni listen
		set->size = list_size(set->list);	//Returnerer størrelsen til listen
	}
	return;
}

/*
 * Returns 1 if the given element is contained in
 * the given set, 0 otherwise.
 */
int set_contains(set_t *set, void *elem)
{
	return list_contains(set->list, elem); //Returnerer 1 om elementet er i listen, 0 hvis ikke
}

/*
 * Returns the union of the two given sets; the returned
 * set contains all elements that are contained in either
 * a or b.
 */
set_t *set_union(set_t *a, set_t *b)
{
	set_t *set3 = set_copy(b); //lager et nytt sett med alle verdier fra sett b
	
	set_iter_t *iter_a = set_createiter(a); //Lager en iterator for elementene i a
	void *elem_a = set_next(iter_a); //Lager en variabel som itererer gjennom settet a
	
	while(elem_a != NULL){ //Så lenge settet a ikke er på NULL
		if(set_contains(set3, elem_a) == 0){ //Sjekk om element i set3/b er ulikt med a
			set_add(set3, elem_a); //Hvis de er ulik så legges de til i set3 
			elem_a = set_next(iter_a); //itererer til neste element i a for videre sammenligning
		}
		else { //Hvis element i a er likt med et element i b
			elem_a = set_next(iter_a); //Itterer til neste element i a
		}
	}
	set_destroyiter(iter_a); //Slett iteratoren

	return set3; //Returner set3
}

/*
 * Returns the intersection of the two given sets; the
 * returned set contains all elements that are contained
 * in both a and b.
 */
set_t *set_intersection(set_t *a, set_t *b)
{
	//Sjekk om verdier i set a og set b er lik, hvis de er lik addes de til set 3
	//Hvis verdiene er ulik gjør ingenting
	set_t *set3 = set_create(a->cmpfunc); //Lager et sett som skal inneholde alle like elementer fra a og b
	set_iter_t *iter_a = set_createiter(a); //lager en iterator for elementer i a
	void *elem_a = set_next(iter_a); //En variabel som itererer gjennom set a
	
	while(elem_a != NULL){ //så lenge det er elementer i a
		if(set_contains(b, elem_a) == 1){ //sjekk om elementer i b er likt med elementer i a
			set_add(set3, elem_a); //Hvis de er lik, legg de til i set 3
			elem_a = set_next(iter_a); //Itererer til neste element i a for sammenligning
		}
		else { //Hvis element i a er ulikt med et element i b
			elem_a = set_next(iter_a); //Iterer til neste element
		}
	}
	set_destroyiter(iter_a); //Ødlegg iteratoren

	return set3; //Returner set3
}

/*
 * Returns the set difference of the two given sets; the
 * returned set contains all elements that are contained
 * in a and not in b.
 */
set_t *set_difference(set_t *a, set_t *b)
{
	set_t *set3 = set_create(set3->cmpfunc); //Lager et sett som skal inneholde elementer som er i a, men ikke de som er lik med b
	set_iter_t *iter_a = set_createiter(a); //Lager en iterator som itererer gjennom elementer i a
	void *elem_a = set_next(iter_a); //En variabel som itererer gjennom set a

	while(elem_a != NULL){ //Så lenge det er elementer i a
		if(set_contains(b, elem_a) == 0) { //Hvis element a er ulikt med det i b
			set_add(set3, elem_a); //Legg elementet til set3
			elem_a = set_next(iter_a); //Iterer til neste element
		}
		else { //Hvis element i a er likt med et element i b
			elem_a = set_next(iter_a); //Itterer til neste element i a
		}
	}
	set_destroyiter(iter_a); //Ødelegg iteratoren
	
	return set3; //Returner set3
}

/*
 * Returns a copy of the given set.
 */
set_t *set_copy(set_t *set)
{
	set_t *newset = set_create(set->cmpfunc); //Lager et nytt sett som skal inneholde elementene i et kopiert sett
	if(newset == NULL) //Sjekker om settet er tomt
		return NULL; //Hvis det er tomt returneres NULL
	

	set_iter_t *iter_copy = set_createiter(set); //Lager en iterator som itererer gjennom et gitt sett
	if(iter_copy == NULL){ //Sjekker om iteratoren er tom
		set_destroy(newset);
		return NULL; //Hvis den er tom returneres NULL
	}

	while(set_hasnext(iter_copy) != 0){ //Sålenge iteratoren ikke er på slutten av settet
		void *item = set_next(iter_copy); //Lager en variabel som returnerer nåværende element og itererer til neste element i et gitt sett
		set_add(newset, item); //Legger elementet inn i det nye settet
	}
	set_destroyiter(iter_copy); //Ødlegger iteratoren
	
	return newset; //Returnerer det nye settet
}

/*
 * The type of set iterators.
 */
//Lager en set iterator
struct set_iter
{
	list_iter_t *list_iter;
};

/*
 * Creates a new set iterator for iterating over the given set.
 */
//funksjonen setter av plass til en sett type (sett_iter_t)
set_iter_t *set_createiter(set_t *set)
{
	set_iter_t *set_iter = malloc(sizeof(set_iter_t));
	if (set_iter == NULL){
		return NULL;
	}

	list_sort(set->list);				//Sorterer listen

	//lager en sett-iterator ved å bruke list_createiter funksjonen og returnerer en ny sett-iterator
	set_iter->list_iter = list_createiter(set->list);
	return set_iter;
}

/*
 * Destroys the given set iterator.
 */
//Sletter sett-iteratoren ved hjelp av list_destroyiter funksjonen og frigir iteratoren
void set_destroyiter(set_iter_t *iter)
{
	list_destroyiter(iter->list_iter);
	free(iter);
}

/*
 * Returns 0 if the given set iterator has reached the end of the
 * set, or 1 otherwise.
 */
//Funksjonen må returnere en verdi og derfor må man skrive return forran funksjonen  
int set_hasnext(set_iter_t *iter)
{
	return list_hasnext(iter->list_iter);
}

/*
 * Returns the next element in the sequence represented by the given
 * set iterator.
 */
void *set_next(set_iter_t *iter)
{
	return list_next(iter->list_iter);
}
