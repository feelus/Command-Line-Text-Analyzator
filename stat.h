/*
 *  Text analysis program
 * 
 *  File: stat.h
 * 
 *  Author: Martin Kucera, 2012
 */


#ifndef STAT_H
#define	STAT_H

#include <stdio.h>
#include "hash_table.h"

/* size of letter frequency array */
#define L_FREQUENCY_SIZE 256

extern word_t *word_table;

/* Structures */

typedef struct {
    char key[3];
    unsigned count;
} letter_t;

/* Function prototypes */

word_t *find_word(char *key);
void add_word(char *key);
void add_word_length(unsigned length);
void add_letter(char *key, unsigned index);
int cmp_letter_frequency(const void *a, const void *b);
void write_stats(FILE *output_file);
void stat_free();

#endif	/* STAT_H */
