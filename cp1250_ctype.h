/*
 *  Text analysis program
 * 
 *  File: cp1250_ctype.h
 * 
 *  Author: Martin Kucera, 2012
 */

#ifndef CP1250_CTYPE_H
#define	CP1250_CTYPE_H

#define _DICTOFFSET	0x80	/* all 7-bit values are same as ASCII */

#define _UPPER          0x1     /* Upper case letter */
#define _LOWER          0x2     /* Lower case letter */
#define _SPACE          0x8     /* Tab, carriage return, newline, vertical tab or form feed */
#define _PUNCT          0x10    /* Punctuation character */
#define _EMPTY		0x20    /* Unused */

/* Function prototypes */

int cp1250_isalpha(int c);
int cp1250_isupper(int c);
int cp1250_islower(int c);
int cp1250_tolower(int c);
int cp1250_isspace(int c);

#endif	/* CP1250_CTYPE_H */

