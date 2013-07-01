/*
 *  Text analysis program
 * 
 *  File: file.c
 *  Provides all file related functions.
 * 
 *  Author: Martin Kucera, 2012
 */

#include <stdio.h>
#include <string.h>

#include "file.h"
#include "err.h"
#include "cp1250_ctype.h"
#include "global.h"

/**
 *  void open_file(FILE **fp, char *name, char *mode)
 * 
 *  Tries to open file with name and mode passed in its parameters, if it succeeds
 *  FILE pointer is saved to fp, if it does not, error is shown with appropriate
 *  message.
 */
void open_file(FILE **fp, char *name, char *mode) {
    char message[200];
    char operation[15];
    
    if((*fp = fopen(name, mode)) == NULL) {        
        if(mode[0] == 'r') {
            strcpy(operation, "read");
        }
        else {
            strcpy(operation, "write to");
        }
        
        sprintf(message, "Couldn't %s file named: %s", operation, name);
        
        raise_error(message);
    }
}

/**
 *  void close_file(FILE **fp)
 * 
 *  Tries to close file associated with fp, if it fails, error is shown.
 */
void close_file(FILE **fp) {
    if(fclose(*fp) != 0) {
	raise_error("Error closing file.");
    }
}

/**
 *  int read_line(FILE *fp, char *buff)
 * 
 *  Reads one line from file. If LBUFFSIZE is smaller than line length, attempts
 *  to move the file pointer before the last word that was split.
 *  
 *  Example: 
 *  LBUFFSIZE = 8
 *  line = test one  
 *  buff = test on
 *  cpos = 7
 *  
 *  Because EOF wasn't reached and \n isnt present in buffer, it means that 
 *  last word was split. We take current position and subtract 
 *  (buffer size - 1) - (position of found space in buffer + 1)
 *  
 *  npos = 7 - ((8 - 1) - (5 + 1))
 */
int read_line(FILE *fp, char *buff) {
    /* char at cpos + 1, has to be signed because of EOF */
    char c;
    /* buffer index */
    int i;
    /* current file position */
    long cpos;
    /* new file position incase line was longer than LBUFFSIZE */
    long npos = 0;
    
    if(fgets(buff, LBUFFSIZE, fp) != NULL) {
	cpos = ftell(fp);
	
        /* got \n, no words were split */
        if((strchr(buff, '\n')) != NULL) {
            return 1;
        }
		
        /* we reached enf of file, but there's still something in the buffer */
        if(feof(fp)) {
            return 1;
        }
        
        /* peek */
        c = fgetc(fp);
        
        /* next char is EOF or space */
        if(c == EOF || cp1250_isspace(c)) {
            return 1;
        }

        /* did not reach EOF or space, need to go one position back */
        fseek(fp, -1, SEEK_CUR);
        
        /* find first space going from the end of the buffer */
        for(i = LBUFFSIZE - 1; i >= 0; i--) {
            if(cp1250_isspace(buff[i])) {
                npos = cpos - ((LBUFFSIZE - 1) - (i + 1));
                buff[i] = '\0';
                break;
            }
        }
        /* indicates that wordsize is bigger than LBUFFSIZE */
        if(npos == 0) {
            raise_error("Wrong text formatting or LBUFFSIZE too small.");
        }
        
        /* move file position in front of the last word that was split */
	fseek(fp, npos, SEEK_SET);

        return 1;  
    }
    
    return 0;
}

/**
 *  void write_line(FILE *fp, char *line)
 * 
 *  Writes line into fp stream, new line is represented by CR LF
 */
void write_line(FILE *fp, char *line) {
    fprintf(fp, "%s%c%c", line, _CR, _LF);
}

/**
 *  long get_file_size(FILE *fp)
 * 
 *  Returns file size, points fp back to original position
 */
long get_file_size(FILE *fp) {
    long end = 0;
    fpos_t current; 
    
    fgetpos(fp, &current);
    
    fseek(fp, 0, SEEK_END);
    end = ftell(fp);
    
    fsetpos(fp, &current);
    
    return end;
}
