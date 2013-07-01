/*
 *  Text analysis program
 * 
 *  File: parser.h
 * 
 *  Author: Martin Kucera, 2012
 */

#ifndef PARSER_H
#define	PARSER_H

/* Function prototypes */

int is_delimiter_outer(char c);
void parse_line(char *ibuff);
int parse_word(char **word);


#endif	/* PARSER_H */
