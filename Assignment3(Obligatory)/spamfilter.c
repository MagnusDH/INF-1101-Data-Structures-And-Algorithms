/* Author: Steffen Viken Valvaag <steffenv@cs.uit.no> */
#include "list.h"
#include "set.h"
#include "array.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

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
static set_t *tokenize(char *filename)
{
	set_t *wordset = set_create(compare_words);
	list_t *wordlist = list_create(compare_words);
	list_iter_t *it;
	FILE *f;
	
	f = fopen(filename, "r");
	if (f == NULL) {
		perror("fopen");
		fatal_error("fopen() failed");
	}
	tokenize_file(f, wordlist);
	
	it = list_createiter(wordlist);
	while (list_hasnext(it)) {
		set_add(wordset, list_next(it));		
	}
	list_destroyiter(it);
	list_destroy(wordlist);
	return wordset;
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
		printf(" %s", (char*)set_next(it)); //THIS WAS ORIGINALY: printf(" %s", set_next(it));
	}
	printf("\n");
	set_destroyiter(it);
}

/*Measures time
(NOT MY ORIGINAL WORK)*/
unsigned long long gettime()
{
	struct timeval tp;
	unsigned long long ctime;

	gettimeofday(&tp, NULL);
	ctime = tp.tv_sec;
	ctime *= 1000000;
	ctime += tp.tv_usec;

	return ctime;
}

/*Main entry point*/
int main (int argc, char **argv)
{	
	/*Measuring time for whole program*/
	long long int time;
	long long int start_time;
	long long int end_time;
	start_time = gettime();

	/*Checking if program is given enough mail directories*/
	if (argc != 4) {
		fprintf(stderr, "usage: %s <spamdir> <nonspamdir> <maildir>\n",
				argv[0]);
		return 1;
	}

/*Names for given files*/
	char *spamdir = argv[1];
	char *nonspamdir = argv[2];
	char *maildir = argv[3];

/*Creating set containing SPAM-words*/
	list_t *spam_files = find_files(spamdir);
	list_iter_t *spam_iter = list_createiter(spam_files);
	void *spam_file = list_next(spam_iter);

	set_t *spam_set = set_create(compare_words);
	set_t *tmp_set = set_create(compare_words);

	while(spam_file != NULL){
		tmp_set = tokenize(spam_file);
		if(set_size(spam_set) == 0){
			spam_set = tmp_set;
		}
		else{
			spam_set = set_intersection(spam_set, tmp_set);
			spam_file = list_next(spam_iter);
		}
	}
	list_destroyiter(spam_iter);
	list_destroy(spam_files);

/*Creating set containing NONSPAM-words*/
	list_t *nonspam_files = find_files(nonspamdir);
	list_iter_t *nonspam_iter = list_createiter(nonspam_files);
	void *nonspam_file = list_next(nonspam_iter);
	set_t *nonspam_set = set_create(compare_words);
	tmp_set = set_create(compare_words);

	while(nonspam_file != NULL){
		tmp_set = tokenize(nonspam_file);
		if(set_size(nonspam_set) == 0){
			nonspam_set = tmp_set;
		}
		else{
			nonspam_set = set_union(nonspam_set, tmp_set);
			nonspam_file = list_next(nonspam_iter);
		}
	}
	list_destroyiter(nonspam_iter);
	list_destroy(nonspam_files);

/*Filtering spam_set and remove common words known NOT to be spam*/
	set_t *word_set = set_difference(spam_set, nonspam_set);

/*Finding which mails contains SPAM-words*/
	list_t *mail_files = find_files(maildir);
	list_sort(mail_files);
	list_iter_t *mail_iter = list_createiter(mail_files);
	char *mail_file = list_next(mail_iter);

	while(mail_file != NULL){
		set_t *tmp_set = tokenize(mail_file);
		set_t *check_set = set_intersection(tmp_set, word_set);
		int size = set_size(check_set);

		if(size >= 1){
			printf("%s: is SPAM and contains %d spam word(s)\n", mail_file, size);
		}
		if(size == 0){
			printf("%s: is NOT SPAM\n", mail_file);
		}
		mail_file = list_next(mail_iter);
	}

	list_destroyiter(mail_iter);
	list_destroy(mail_files);
	set_destroy(tmp_set);


	/*Ending time-measurement for program*/
	end_time = gettime();
	time = end_time - start_time;
	printf("Time for program to execute is: %lld microseconds\n", time);

    return 0;
}