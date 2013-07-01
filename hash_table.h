/*
 *  Text analysis program
 * 
 *  File: hash_table.h
 * 
 *  Author: Martin Kucera, 2012
 */

#ifndef HASH_TABLE_H
#define	HASH_TABLE_H

#include <stddef.h>

/* Maximum number of buckets a table can have */
#define BUCKET_NUM_MAX 2097152
/* Maximum number of items in one bucket before expand */
#define BUCKET_NUM_TRESH 10

/* Maximum size for table item's key */
#define KEY_MAX_LEN 512

/* Prototypes */

typedef struct word word_t;
typedef struct hash_bucket hash_bucket_t;
typedef struct hash_handle hash_handle_t;
typedef struct hash_table hash_table_t;

/* Structures */

struct hash_bucket {
    hash_handle_t *head;
    unsigned long num;
    int noexpand;
};

struct hash_handle {
    hash_table_t *table;
    hash_handle_t *next;    
    hash_handle_t *next_w;    
    
    unsigned long hash;
    unsigned keylen;
};

struct hash_table {
    hash_bucket_t *buckets;
    ptrdiff_t hhoffset;
    
    hash_handle_t *tail;
    
    unsigned long count;
    unsigned long num;
};

struct word {
    char *key;
    unsigned count;
    hash_handle_t hh;
};

/* Function prototypes */

void hash_set_count(long count);
void hash_guess_count(long count);
unsigned long hash_jen(char *key, unsigned len);
unsigned long hash_get_index(unsigned long hash, unsigned long table_size);
void hash_create_table(word_t *head);
void hash_expand_buckets(hash_table_t *table);
void hash_add_to_bkt(hash_bucket_t *bkt, hash_handle_t *hh);
void hash_add_str(word_t **head, char *key, word_t *item, unsigned keylen);
void hash_find_in_bkt(hash_table_t *table, hash_bucket_t *bkt, char *key, unsigned keylen, word_t **out);
void hash_find_str(word_t *head, char *key, word_t **out);
void hash_get_next(word_t *head, word_t **out);
void hash_free_table(word_t **head);
unsigned long hash_count(word_t *head);
void hash_sort(word_t **head);
void hash_print_debug(word_t *head);


#endif	/* HASH_TABLE_H */
