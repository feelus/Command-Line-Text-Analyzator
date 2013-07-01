/*
 *  Text analysis program
 * 
 *  File: file.c
 *  Keeps statistic of all words, letters, calculates word length frequency and
 *  outputs final stats to a file.
 * 
 *  Author: Martin Kucera, 2012
 */


#include <stdlib.h>
#include <string.h>

#include "stat.h"
#include "hash_table.h"
#include "global.h"
#include "file.h"

/* hash table head for all words */
word_t *word_table = NULL;

/* maximum length of a word */
unsigned w_length_max = 0;

/* size of w_lengths array */
unsigned w_lengths_size = 15;
/* array with frequency of word lengths */
unsigned *w_lengths = NULL;

/* array of letters and their frequencies */
letter_t *l_frequency;
/* total number of letters */
unsigned long l_total = 0;

/**
 *  word_t *find_word(char *key)
 * 
 *  Attempts to find word_t with key from parameter in hash table.
 *  If found, returns it's pointer, else returns NULL.
 */
word_t *find_word(char *key) {
    word_t *w;
    
    hash_find_str(word_table, key, &w);
    
    return w;
}

/** 
 *  void add_word(char *key)
 * 
 *  Adds word into hash table. If word already exists, increases it's count. If 
 *  it does not, allocates memory for new word and it's key.
 * 
 *  Saves each new word's length and finds the maximum word length. 
 */
void add_word(char *key) {
    word_t *w;
    char *d;
    unsigned length;

    length = strlen(key);    
    w = find_word(key);
        
    if(w == NULL) {
        if(length > w_length_max)
            w_length_max = length;
        
        add_word_length(length);
        
	w = (word_t *) malloc(sizeof(word_t));
	d = (char *) malloc(length + 1);
	
	strcpy(d, key);
	
	w->key = d;
	w->count = 1;
	
        hash_add_str(&word_table, w->key, w, length);
    }
    else {
	w->count++;
    }
}

/**
 *  void add_word_length(unsigned length)
 * 
 *  Adds length to word length array. If length is larger than current
 *  w_length's size, then current size is doubled.
 */
void add_word_length(unsigned length) {
    unsigned old_size = 0;
    
    if(w_lengths == NULL || length > w_lengths_size) {
        if(w_lengths != NULL)
            old_size = w_lengths_size;
        
        if(length > w_lengths_size)
            /* if new word length (index) is more than twice w_lengths_size, length is used */
            w_lengths_size = ((w_lengths_size * 2) >= length) ? (w_lengths_size * 2) : length;
        
        w_lengths = (unsigned *) realloc(w_lengths, w_lengths_size * sizeof(unsigned));
        
        memset((w_lengths + old_size), 0, (w_lengths_size - old_size) * sizeof(unsigned));
    }
    
    w_lengths[length - 1]++;
}

/**
 *  void add_letter(char *key, unsigned index)
 * 
 *  Increments the counter for letter at index passed in arguments. If this letter
 *  hasn't been initialized yet, sets it's key.
 * 
 *  Czech letter ch is kept at unused index 0
 */
void add_letter(char *key, unsigned index) {
    l_total++;
    
    if(l_frequency == NULL) {
        l_frequency = (letter_t *) malloc(sizeof(letter_t) * L_FREQUENCY_SIZE);
        memset(l_frequency, 0, sizeof(letter_t) * L_FREQUENCY_SIZE);
    }
    
    if(l_frequency[index].key[0] == 0)
        strcpy(l_frequency[index].key, key);
    
    l_frequency[index].count++;
}

/**
 *  int cmp_letter_frequency(const void *a, const void *b)
 * 
 *  Compares two letters, returns value less than zero, zero or greater than zero
 *  if a goes before, is equal or goes after b.
 */
int cmp_letter_frequency(const void *a, const void *b) {
    letter_t la = *(letter_t *) a;
    letter_t lb = *(letter_t *) b;
    
    return (lb.count - la.count);
}

/**
 *  void write_stats(FILE *output_file)
 *  
 *  Writes final stats to output_file
 */
void write_stats(FILE *output_file) {
    char buff[OBUFFSIZE];
    int i;
    word_t *w = NULL;
    double relative_frequency = 0;
    
    if(hash_count(word_table) == 0) {
        write_line(output_file, "There were no words in input file.");
        exit(EXIT_SUCCESS);
    }
    
    /* total number of words */
    sprintf(buff, "#words %lu", hash_count(word_table));    
    write_line(output_file, buff);

    /* maximum length of a word */
    sprintf(buff, "#maxlen %u", w_length_max);    
    write_line(output_file, buff);
    
    /* word lengths frequency */
    for(i = 0; i < w_length_max; i++) {
        sprintf(buff, "#len(%d) %u", (i + 1), w_lengths[i]);
        write_line(output_file, buff);
    }
    
    write_line(output_file, "%%%");
    
    /* sort words by their frequencies */
    hash_sort(&word_table);
    
    /* all words and their frequencies */
    hash_get_next(word_table, &w);    
    while(w != NULL) {
        sprintf(buff, "%s %u", w->key, w->count);
        write_line(output_file, buff);
        
        hash_get_next(word_table, &w);
    }
    
    write_line(output_file, "%%%");
    
    if(l_frequency != NULL) {
        /* sort letters by their frequencies DESC */
        qsort(l_frequency, L_FREQUENCY_SIZE, sizeof(letter_t), cmp_letter_frequency);
        
        /* all letters and their frequencies */
        for(i = 0; i < L_FREQUENCY_SIZE; i++) {
            if(l_frequency[i].count > 0) {
                relative_frequency = (double) l_frequency[i].count / l_total;
                sprintf(buff, "%s %.8f", l_frequency[i].key, relative_frequency);
                write_line(output_file, buff);
            }
        }
    }
}

/**
 *  void stat_free()
 * 
 *  Frees frequency array, word lengths and hash table.
 */
void stat_free() {
    if(l_frequency != NULL) {
        free(l_frequency);
        l_frequency = NULL;
    }
    
    if(w_lengths != NULL) {
        free(w_lengths);    
        w_lengths = NULL;
    }
        
    hash_free_table(&word_table);
}
