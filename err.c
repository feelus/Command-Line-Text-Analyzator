/*
 *  Text analysis program
 * 
 *  File: err.c
 *  Error handling function(s)
 * 
 *  Author: Martin Kucera, 2012
 */

#include <stdio.h>
#include <stdlib.h> 

/**
 *  void raise_error(char *msg)
 * 
 *  Interrupts program execution, prints error message and exits with EXIT_FAILURE
 *  code.
 */
void raise_error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}
