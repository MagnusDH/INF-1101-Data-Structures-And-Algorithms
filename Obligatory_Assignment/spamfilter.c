/* Author: Steffen Viken Valvaag <steffenv@cs.uit.no> */
#include "list.h"
#include "set.h"
#include "common.h"
#include <sys/time.h>

//////////////////////////////////
/*Funksjon som tar tiden*/		//
unsigned long long gettime(void)//
{								//
    struct timeval tp;			//
    unsigned long long ctime;	//
    							//
    gettimeofday(&tp, NULL);	//
    ctime = tp.tv_sec;			//
    ctime *= 1000000;			//
    ctime += tp.tv_usec;		//
    							//
    return ctime;				//
}								//
//////////////////////////////////

/*
 * Case-insensitive comparison function for strings.
 */
static int compare_words(void *a, void *b)
{
    return strcasecmp(a, b);
}

/*
 * Returns the set of (unique) words found in the given file.
 */
static set_t *tokenize(char *filename)	//Returnerer ett set med ord fra én fil
{
	set_t *wordset = set_create(compare_words);		//Lager et sett "wordset"
	list_t *wordlist = list_create(compare_words);	//Lager en liste "wordlist"
	list_iter_t *it;								//Oppretter en type kallt "it"
	FILE *f;										//
	
	f = fopen(filename, "r");						//Setter f til å åpne en fil
	if (f == NULL) {								//Hvis fila er tom
		perror("fopen");							//error
		fatal_error("fopen() failed");
	}
	tokenize_file(f, wordlist);						//Tokenize_file får adressen til en fil "f" og en liste "wordlist". Funksjonen tar ordene fra settet "f" og putter de inn i listen "wordlist"
	
	it = list_createiter(wordlist);					//"it" er navnet på en listeiterator som går gjennom "wordlist" 
	while (list_hasnext(it)) {						//Så lenge "wordlist" har elementer itererer den gjennom listen
		set_add(wordset, list_next(it));			//List_next returnerer et Elementet/ord som blir lagt til i settet "wordset" 
	} 
	list_destroyiter(it);							//iteratoren til "it" blir slettet
	list_destroy(wordlist);							//Listen til wordlist blir slettet
	return wordset;									//Og det nye settet "wordset" blir returnert.
}

/*
 * Prints a set of words.
 */
static void printwords(char *prefix, set_t *words)
{
	set_iter_t *it;
	
	it = set_createiter(words);
	printf("%s: ", prefix);
	while (set_hasnext(it)) {
		printf(" %s", (char*)set_next(it));
	}
	printf("\n");
	set_destroyiter(it);
}


/*
 * Main entry point.
 */
int main(int argc, char **argv)
{
	char *spamdir, *nonspamdir, *maildir;
	
	if (argc != 4) {
		fprintf(stderr, "usage: %s <spamdir> <nonspamdir> <maildir>\n",
				argv[0]);
		return 1;
	}
	spamdir = argv[1];
	nonspamdir = argv[2];
	maildir = argv[3];

//////////////////////////////
long long int time;			//
long long int starttime;	//
long long int endtime;		//
starttime = gettime();		//
//////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Sett som inneholder kun spam-ordene fra spamfiles */

	list_t *spamfiler = find_files(spamdir);			//Laster inn filer fra spamdir og returnerer de 4 filnavnene i en liste kalt spamfiler
	list_iter_t *spamiter = list_createiter(spamfiler);	//Lager en iterator for spamfiler listen
	void *item = list_next(spamiter); 					//setter item til å være første element i spamiteratoren 
	
	set_t *spam_set = set_create(compare_words);		//Lager et nytt sett "spam_set" som skal inneholde alle spam-ord
	set_t *tmp_set = set_create(compare_words);			//Lager en variabel tmp_set som skal brukes i while-loopen
		
	while(item != NULL){								//Itererer gjennom alle filene (tmp) frem til den når NULL
		tmp_set = tokenize(item);						//Leser gitt fil og setter ordene inn i tmp_set
		if(set_size(spam_set) == 0){					//Hvis settet er tomt
			spam_set = tmp_set;							//så kopieres ordene fra tmp_set til spam_set
		}
		else{											//Hvis settet ikke er tomt
			spam_set = set_intersection(spam_set, tmp_set);	//Sjekker om elementer i spam_set og tmp_set er lik, hvis de er lik addes de til spam_set
			item = list_next(spamiter);						//Itererer til neste fil
		}
	}

	list_destroyiter(spamiter);




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/* Alle ord fra non-spam flyttes til et STORT sett */ 
	list_t *nonspamfiler = find_files(nonspamdir);			//Laster inn filer fra nonspamdir og returnerer de 4filnavnene som en liste 
	list_iter_t *nonspamiter = list_createiter(nonspamfiler);//Lager en iterator for nonspamfiler i listen
	void *item2 = list_next(nonspamiter); 					//Setter item2 til å være første element i nonspamiteratoren
	set_t *nonspam_set = set_create(compare_words);			//Lager et sett "nonspam_set" som skal inneholde alle nonspam-ord
	set_t *new_set = set_create(compare_words); 

	while (item2 != NULL){
		tmp_set = tokenize(item2);					//Leser gitt fil og setter ordene inn i tmp_set
		nonspam_set = set_union(new_set, tmp_set);	//setter alle ord fra nonspam_set og tmp_set inn i new_set
		item2 = list_next(nonspamiter);				//Itererer til neste fil
	}

	list_destroyiter(nonspamiter);							//Sletter iteratoren
	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



 //Settet med spam-ord sammenlignes med nonspam-settet, for å fjerne alle vanlige ord som kan forekomme i en spam mail */

	set_t *word_set = set_difference(spam_set, nonspam_set);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* settet med de ukjente mailene minus wordset */ 
	list_t *mailfiler = find_files(maildir);			//Laster inn filer fra maildir og returnerer 5filer som en liste
	list_sort(mailfiler);									
	list_iter_t *mailiter = list_createiter(mailfiler);	//Lager en iterator for mailfiler i listen
	char *item3 = list_next(mailiter);					//Setter item3 til å være første element i mailiteratoren	

	while (item3 != NULL){
		set_t *tmp_set = tokenize(item3);						//Leser gitt fil og setter ordene inn i tmp_set
		new_set = set_intersection(tmp_set, word_set);			//Sjekker om elementer i mailen er inneholdt i word_set(set med spamord), hvis de er lik addes de til new_set
		int size = set_size(new_set);							//Sjekker størrelsen på new_set
		

		if(size >= 1){
			printf("%s is spam and contains %d spam-words\n", item3, size);
		}
		if(size == 0){
			printf("%s is not spam\n", item3);
		}

		item3 = list_next(mailiter);							//Itererer til neste fil i listen	
	}	
	set_destroy(new_set);
	set_destroy(tmp_set);
	list_destroyiter(mailiter);

////////////////////////////////////////////////////////////
endtime = gettime();	   								  //
time = endtime - starttime;								  //
printf("tiden på programmet er %lld mikrosekund\n", time);//
    return 0;											  //
////////////////////////////////////////////////////////////

}
