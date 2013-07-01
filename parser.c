/*
 *  Text analysis program
 * 
 *  File: parser.c
 *  Handles line and word parsing. Splits input line by defined delimiters, then
 *  validates each word. If word is successfuly validated, passes the word and
 *  all it's letters into stat.c.
 * 
 *  Author: Martin Kucera, 2012
 */

#include <stdio.h>
#include <string.h>

#include "cp1250_ctype.h"
#include "stat.h"
#include "parser.h"

/* All delimiters */
unsigned char delimiters[] = {
    9,		/* HT */
    10,		/* LF */
    11,		/* VT */
    13,		/* CR */
    32,		/* SP */
    33,		/* ! */
    34,		/* " */
    36,		/* $ */
    37,		/* % */
    40,		/* ( */
    41,		/* ) */
    44,		/* , */
    45,         /* - */
    46,		/* . */
    48,         /* 0 */
    49,         /* 1 */
    50,         /* 2 */
    51,         /* 3 */
    52,         /* 4 */
    53,         /* 5 */
    54,         /* 6 */
    55,         /* 7 */
    56,         /* 8 */
    57,         /* 9 */
    58,		/* : */
    59,		/* ; */
    63,		/* ? */
    64,		/* @ */
    91,		/* [ */
    93,		/* ] */
    123,	/* { */
    125,	/* } */
    126,	/* ~ */
    130,	/* ‚ */
    132,	/* „ */
    133,	/* … */
    137,	/* ‰ */
    145,	/* ‘ */
    146,        /* ’ */
    147,	/* “ */
    148, 	/* ” */
    '\0'
};

/* Outer delimiters */
unsigned char delimiters_outer[] = {
    39,          /* ' */
    '\0'
};

/**
 *  int is_delimiter_outer(char c)
 * 
 *  Checks if c is an outer delimiter. Outer delimiters are allowed inside a 
 *  word.
 */
int is_delimiter_outer(char c) {
    int length, i;
    
    length = strlen((char *)delimiters_outer);
    
    for(i = 0; i < length; i++) {
        if(delimiters_outer[i] == c)
            return 1;
    }
    
    return 0;
}

/**
 *  void parse_line(char *ibuff)
 * 
 *  Splits ibuff using strtok and passes each word to parse_word
 */
void parse_line(char *ibuff) {
    char *pc;

    pc = strtok(ibuff, (char *)delimiters);
    while(pc != NULL) {
	if(parse_word(&pc)) {
	    add_word(pc);
	}
	pc = strtok(NULL, (char *)delimiters);
    }
}

/**
 *  int parse_word(char **word)
 * 
 *  Parses individual strings, converts all alphabetical letters to it's
 *  lowercase equivalent. Word is allowed to have any delimiters_outer inside, 
 *  but cannot be beginning or ending characters, those are stripped.
 * 
 *  If string passes parsing and is considered a word, each of his letters are passed
 *  to add_letter.
 *  Characters c and h together - ch are considered as one in Czech language.
 */
int parse_word(char **word) {
    int i, count, length, od_index, od_count, shifts;
    char d[3];
    unsigned index;
    
    count = od_index = od_count = shifts = 0;
    length = strlen((*word));
    
    for(i = 0; i < length; i++) {
        if(is_delimiter_outer((*word)[i])) {
            if(count == 0 && ((i + shifts + 1) < length)) {
                shifts++;
                (*word)++;
            }
            else {
                od_count = count;
                od_index = i;
            }
            
            continue;
        }
        
        if(cp1250_isalpha((unsigned char) (*word)[i])) {
            (*word)[i] = cp1250_tolower((unsigned char) (*word)[i]);
            
            if(((i + 1) < length) && (*word)[i] == 'c' && (*word)[i + 1] == 'h') {
                strcpy(d, "ch");
                index = 0;
                i++;
            }
            else {
                d[0] = (*word)[i];
                d[1] = '\0';
                index = (unsigned char) (*word)[i];
            }

            add_letter(d, index);
            count++;
        }
    }
    
    if(od_count == count)
        (*word)[od_index] = '\0';
    
    return (count > 0);
}
