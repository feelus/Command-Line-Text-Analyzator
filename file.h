/*
 *  Text analysis program
 * 
 *  File: file.h
 * 
 *  Author: Martin Kucera, 2012
 */

#ifndef FILE_H
#define	FILE_H

#define _CR 0x0D
#define _LF 0x0A

/* Function rototypes */

void open_file(FILE **fp, char *name, char *mode);
void close_file(FILE **fp);
int read_line(FILE *fp, char *buff);
void write_line(FILE *fp, char *line);
long get_file_size(FILE *fp);


#endif	/* FILE_H */
