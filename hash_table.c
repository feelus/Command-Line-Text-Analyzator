/*
 *  Text analysis program
 * 
 *  File: hash_table.c
 *  A hash table using separate chaining. Stores structures word_t containing
 *  word's key and it's count. Table expands when in any of it's
 *  buckets is more than BUCKET_NUM_TRESH items. Maximum number of buckets is 
 *  set using BUCKET_NUM_MAX, there will be no more expands when this number is
 *  reached. When expand is in progress and new bucket size would exceed this 
 *  limit, new bucket count is set to BUCKET_NUM_MAX. During expand all items
 *  are reindexed and placed into new table.
 * 
 *  Inspired by uthash, rewritten and simplified
 * 
 *  Copyright (c) 2003-2013, Troy D. Hanson     http://uthash.sourceforge.net
 * 
 *  Author: Martin Kucera, 2012
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "err.h"
#include "hash_table.h"

/* starting bucket size */
unsigned BUCKET_INIT_COUNT = 32;
/* table expand flag */
unsigned FLAG_EXPAND = 1;

/** void hash_set_count(long count)
 * 
 *  Sets the initial bucket count, similiar to hash_guess_count, but sets the
 *  number directly
 */
void hash_set_count(long count) {
    if(count == 0 || count > BUCKET_NUM_MAX) 
        return;
    
    BUCKET_INIT_COUNT = count;
}

/**
 *  void hash_guess_count(long count)
 *  
 *  EXPERIMENTAL
 * 
 *  Changes the initial bucket count. Can decrease execution time
 *  or increase memory efficiency or the opposite. Potentionaly dangerous.
 */
void hash_guess_count(long count) {
    unsigned long count_u;
    float tmp;
    
    /* assume average word length is 8 and each word occurs more than once */
    tmp = count / 8;
    tmp = ceil(tmp * 0.05);
    
    count_u = (unsigned long) tmp;
    
    if(count_u <= 32 || count <= 32) {
        return;
    }
    
    /* get nearest upper power of two */
    count_u--;
    count_u |= count_u >> 1;
    count_u |= count_u >> 2;
    count_u |= count_u >> 4;
    count_u |= count_u >> 8;
    count_u |= count_u >> 16;
    count_u++;
    
    hash_set_count(count_u);
}

/** unsigned long hash_jen(char *key, unsigned len)
 * 
 *  Basic non-cryptographic hashing function for strings
 *  called one-at-a-time taken from wikipedia (designed by Bob Jenkins),
 *  takes a string and it's length as parameters.
 */
unsigned long hash_jen(char *key, unsigned len) {
    unsigned i;
    unsigned long hash;
    
    for(hash = i = 0; i < len; i++) {
      hash += key[i];
	    hash += (hash << 10);
	    hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

/** unsigned long hash_get_index(unsigned long hash, unsigned long table_size)
 * 
 *  Returns bucket index based on given hash and table size.
 */
unsigned long hash_get_index(unsigned long hash, unsigned long table_size) {
    return ((hash) & (table_size - 1));
}

/**
 *  void hash_create_table(word_t *head)
 * 
 *  Creates a hash table with head being the first inserted item.
 *  Stores tail reference for fast append to the list of items.
 *  Allocates memory for table itself and it's number of buckets,
 *  which is set by BUCKET_INIT_COUNT and sets all bytes of allocated
 *  space to zeros.
 */
void hash_create_table(word_t *head) {
    (head)->hh.table = (hash_table_t *) malloc(sizeof(hash_table_t));

    if(!(head)->hh.table) {
        raise_error("Out of memory.");
    }
    
    (head)->hh.table->count = BUCKET_INIT_COUNT;
    (head)->hh.table->num = 0;
    (head)->hh.table->tail = &((head)->hh);
    (head)->hh.table->hhoffset = offsetof(word_t, hh); /* offset of hash_handle inside of inserted item */
    (head)->hh.table->buckets = (hash_bucket_t *) malloc(sizeof(hash_bucket_t) * BUCKET_INIT_COUNT);
    
    if(!(head)->hh.table->buckets) {
        raise_error("Out of memory.");
    }
    
    memset((head)->hh.table->buckets, 0, sizeof(hash_bucket_t) * BUCKET_INIT_COUNT);    
}

/**
 *  void hash_expand_buckets(hash_table_t *table)
 * 
 *  Called when one of the buckets exceeds BUCKET_NUM_TRESH and it's noexpand flag
 *  is not set. Doubles the previous size of buckets, recalculates new bucket indexes
 *  for each item in old table and frees the old bucket space.
 */
void hash_expand_buckets(hash_table_t *table) {
    /* bucket index in original buckets */
    unsigned long bkt_i;
    /* current hash_handle */
    hash_handle_t *chh;
    /* next hash_handle */
    hash_handle_t *nhh;
    /* pointer to the newly allocated space for buckets */
    hash_bucket_t *new_buckets;
    /* new bucket in buckets_new */
    hash_bucket_t *new_bucket;
    /* new bucket size */
    unsigned long new_bucket_count;
    
    new_bucket_count = 2 * table->count;
    
    if(new_bucket_count > BUCKET_NUM_MAX)
        new_bucket_count = BUCKET_NUM_MAX;
    
    /* if current table size is >= than max. number of buckets
     * set FLAG_EXPAND to zero and return
     */
    if((table->count >= BUCKET_NUM_MAX)) {
        FLAG_EXPAND = 0;
        return;
    }

    new_buckets = (hash_bucket_t *) malloc((sizeof(hash_bucket_t) * new_bucket_count));
    
    if(!new_buckets) {
        raise_error("Out of memory.");
    }
    
    memset(new_buckets, 0, (sizeof(hash_bucket_t) * new_bucket_count));
    
    /* go through all buckets and reindex each item in bucket to the newly allocated space */
    for(bkt_i = 0; bkt_i < table->count; bkt_i++) {
        chh = table->buckets[bkt_i].head;
        
        while(chh) {
            nhh = chh->next;
            new_bucket = &(new_buckets[hash_get_index(chh->hash, table->count * 2)]);
            
            new_bucket->num++;
            
            if(new_bucket->num >= BUCKET_NUM_TRESH) {
                new_bucket->noexpand = 1;                
            }
            
            chh->next = new_bucket->head;
            new_bucket->head = chh;
            
            chh = nhh;
        }
    }
    
    free(table->buckets);
    
    table->count *= 2;
    table->buckets = new_buckets;
}

/** 
 *  void hash_add_to_bkt(hash_bucket_t *bkt, hash_handle_t *hh)
 * 
 *  Adds item's hash_handle into given bucket. Appending in insertion order,
 *  first item inserted into bucket becomes last item in the linked list.
 */
void hash_add_to_bkt(hash_bucket_t *bkt, hash_handle_t *hh) {
    bkt->num++;
    
    hh->next = bkt->head;
    bkt->head = hh;
    
    if(bkt->num >= BUCKET_NUM_TRESH && bkt->noexpand == 0 && FLAG_EXPAND) {
        hash_expand_buckets(hh->table);
    }
}

/**
 *  void hash_add_str(word_t **head, char *key, word_t *item, unsigned keylen)
 * 
 *  Adds item with string key into the hash table. If hash table hasn't
 *  been initialized yet, calls hash_create_table with head being the inserted
 *  item. 
 */
void hash_add_str(word_t **head, char *key, word_t *item, unsigned keylen) {
    unsigned long hash;
    
    if(keylen > KEY_MAX_LEN) {
        return;
    }
    
    if(!(*head)) {
        (*head) = item;
        hash_create_table((*head));
    }
    else {
        (*head)->hh.table->tail->next_w = &((item)->hh);
        (*head)->hh.table->tail = &((item)->hh);
    }
    
    hash = hash_jen(key, keylen);
    
    (*head)->hh.table->num++;
    item->hh.table = (*head)->hh.table;
    item->hh.keylen = keylen;
    item->hh.hash = hash;
    item->hh.next_w = NULL;
    
    /* inserts item hash_handle into a bucket, index calculated using hash_get_index */
    hash_add_to_bkt(
            &((*head)->hh.table->buckets[hash_get_index(
                hash,
                (*head)->hh.table->count
            )]),
            &(item->hh)
    );
}

/**
 *  word_t* hash_elmt_from_hh(hash_table_t *table, hash_handle_t *hh)
 * 
 *  Returns pointer to a word_t structure calculated using hash_handle offset.
 */
word_t* hash_elmt_from_hh(hash_table_t *table, hash_handle_t *hh) {
    if(!table || !hh) {
        return NULL;
    }
    
    return ((word_t *)(((char*)(hh)) - ((table)->hhoffset)));
}

/** void hash_find_in_bkt(hash_table_t *table, hash_bucket_t *bkt, char *key, unsigned keylen, word_t **out)
 * 
 *  Attempts to find given key in a given bucket, first compares key length, then
 *  actual keys, output pointer is saved to out variable. Returns NULL if item
 *  wasn't found.
 */
void hash_find_in_bkt(hash_table_t *table, hash_bucket_t *bkt, char *key, unsigned keylen, word_t **out) {
    if(bkt->head) {
        (*out) = hash_elmt_from_hh(table, bkt->head);
    }
    else {
        (*out) = NULL;
    }
    
    while((*out)) {
        if((*out)->hh.keylen == keylen) {
            if(strncmp((*out)->key, key, keylen) == 0) {
                return;
            }
        }
        
        if((*out)->hh.next) {
            (*out) = hash_elmt_from_hh(table, (*out)->hh.next);
        }
        else {
            (*out) = NULL;
        }
    }
}

/**
 *  void hash_find_str(word_t *head, char *key, word_t **out)
 * 
 *  Attempts to find given key in hash table. Output is saved into
 *  variable out. Returns NULL if key wasn't found.
 */
void hash_find_str(word_t *head, char *key, word_t **out) {
    unsigned long hash;
    unsigned keylen;
    
    (*out) = NULL;
    
    if(head) {
        keylen = strlen(key);
        hash = hash_jen(key, keylen);
        
        hash_find_in_bkt(
                (head)->hh.table,
                &((head)->hh.table->buckets[hash_get_index(
                        hash,
                        (head)->hh.table->count
                )]),
                key,
                keylen,
                out
        );
    }
}

/**
 *  void hash_get_next(word_t *head, word_t **out)
 * 
 *  Finds next item in appending order (next item after variable 'out'),
 *  if called with 'out' pointing to NULL, returns first item (head).
 */
void hash_get_next(word_t *head, word_t **out) {
    if(!(head)) {
        return;
    }
    
    if(!(*out)) {
        (*out) = head;
    }
    else {
        (*out) = hash_elmt_from_hh((head)->hh.table, (*out)->hh.next_w);
    }
}

/**
 *  void hash_free_table(word_t **head)
 * 
 *  Frees all memory allocated by inserted items and table itself, afterwards
 *  sets head to NULL.
 */
void hash_free_table(word_t **head) {    
    word_t *current;
    word_t *next;
    hash_table_t *table;
    
    if(!(*head)) 
        return;
    
    table = (*head)->hh.table;
    current = (*head);
    
    while(current) {
        next = hash_elmt_from_hh(table, (current->hh.next_w));
        
        free(current->key);
        free(current);
        
        current = next;
    }

    free(table->buckets);
    free(table);
    
    (*head) = NULL;
}

/**
 *  unsigned long hash_count(word_t *head)
 * 
 *  Returns number of items in table.
 */
unsigned long hash_count(word_t *head) {
    if(head) {
        return (head->hh.table->num);
    }
    
    return 0;
}

/**
 *  void hash_sort(word_t **head)
 * 
 *  Sorting function taken directly from uthash, adaptation of mergesort with O(n log(n))
 *  complexity. 
 */
void hash_sort(word_t **head) {
  unsigned long _hs_i;
  unsigned long _hs_looping,_hs_nmerges,_hs_insize,_hs_psize,_hs_qsize;
  hash_handle_t *_hs_p, *_hs_q, *_hs_e, *_hs_list, *_hs_tail;            
  
  if((*head)) {
      _hs_insize = 1;
      _hs_looping = 1;
      _hs_list = &((*head)->hh);
      
      while(_hs_looping) {
          _hs_p = _hs_list;
          _hs_list = NULL;
          _hs_tail = NULL;
          _hs_nmerges = 0;
          
          while(_hs_p) {
              _hs_nmerges++;
              _hs_q = _hs_p;
              _hs_psize = 0;
              
              for(_hs_i = 0; _hs_i < _hs_insize; _hs_i++) {
                  _hs_psize++;
                  _hs_q = (_hs_q->next_w) ? _hs_q->next_w : NULL;
                  if(!(_hs_q)) break;
              }
              
              _hs_qsize = _hs_insize;
              
              while((_hs_psize > 0) || ((_hs_qsize > 0) && _hs_q)) {
                  if(_hs_psize == 0) {
                      _hs_e = _hs_q;
                      _hs_q = (_hs_q->next_w) ? _hs_q->next_w : NULL;
                      _hs_qsize--;
                  }
                  else if((_hs_qsize == 0) || !(_hs_q)) {
                      _hs_e = _hs_p;
                      _hs_p = (_hs_p->next_w) ? _hs_p->next_w : NULL;
                      _hs_psize--;
                  }
                  else if(
                          hash_elmt_from_hh((*head)->hh.table, _hs_p)->count >=
                          hash_elmt_from_hh((*head)->hh.table, _hs_q)->count
                          ) {
                      _hs_e = _hs_p;
                      _hs_p = (_hs_p->next_w) ? _hs_p->next_w : NULL;
                      _hs_psize--;
                  }
                  else {
                      _hs_e = _hs_q;
                      _hs_q = (_hs_q->next_w) ? _hs_q->next_w : NULL;
                      _hs_qsize--;
                  }
                  if(_hs_tail) {
                      _hs_tail->next_w = (_hs_e) ? _hs_e : NULL;
                  }
                  else {
                      _hs_list = _hs_e;
                  }
                  _hs_tail = _hs_e;
              }
              _hs_p = _hs_q;
          }
          _hs_tail->next_w = NULL;
          
          if(_hs_nmerges <= 1) {
              _hs_looping = 0;
              ((*head))->hh.table->tail = _hs_tail;
              (*head) = hash_elmt_from_hh((*head)->hh.table, _hs_list);
          }
          _hs_insize *= 2;
      }
  }
}

/**
 *  void hash_print_debug(word_t *head)
 * 
 *  Prints table debug info.
 */
void hash_print_debug(word_t *head) {
    unsigned bkt_i;
    hash_handle_t *chh;
    word_t *word;
    
    printf("##############################################\n");
    
    printf("TABLE SIZE: %lu\n", head->hh.table->count);    
        
    for(bkt_i = 0; bkt_i < head->hh.table->count; bkt_i++) {
        chh = head->hh.table->buckets[bkt_i].head;
        if(!chh) continue;
        printf("-------------------------------------\n");
        printf("ENTERING BUCKET ID: %u, NUM: %lu\n", bkt_i, head->hh.table->buckets[bkt_i].num);
        
        while(chh) {
            word = hash_elmt_from_hh(head->hh.table, chh);
            printf("word -- data: %s, count: %u\n", word->key, word->count);
            
            chh = chh->next;            
        }
    }
}
