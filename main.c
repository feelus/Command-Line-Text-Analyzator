/*
 *  Text analysis program
 * 
 *  File: main.c
 *  Main file, reads command line arguments, initiates all processes needed.
 *  When done, handles memory freeing and closes any open files.
 * 
 *  Author: Martin Kucera, 2012
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "stat.h"
#include "file.h"
#include "err.h"
#include "parser.h"
#include "global.h"
#include "hash_table.h"

FILE *input_file;
FILE *output_file;

/**
 *  long get_str_number(char *string)
 * 
 *  Converts string to long.
 */
long get_str_number(char *string) {
    char *p;
    long i = strtol(string, &p, 10);
    
    if((string[0] != '\n') && (*p == '\n' || *p == '\0')) {
        return i;
    }
    else {
        return 0;
    }
}

/**
 *  void process_input()
 * 
 *  Reads and parses input file. If there were no data present, raises error.
 */
void process_input() {
    char buff[LBUFFSIZE];
    unsigned read_lines = 0;
    
    printf("Parsing input ...\n");
    
    while(read_line(input_file, buff)) {
	parse_line(buff);
        read_lines++;
    }
    
    if(read_lines == 0)
        raise_error("Input file is empty.");
}

/**
 *  void help()
 * 
 *  Prints brief help, basic program usage.
 */
void help() {
    printf("NAME:\n");
    printf("\t\t csstat.exe - Text analysis tool.\n");
    
    printf("--------------------------------------------------\n");
    printf("USAGE:\n");
    printf("\t\t csstat.exe {inpf} {outf} [init bucket size]\n");
    
    printf("--------------------------------------------------\n");
    printf("EXAMPLE:\n");
    printf("\t\t csstat.exe input.txt out.stat\n");
    printf("\t\t csstat.exe input.txt out.stat guess\n");
    printf("\t\t csstat.exe input.txt out.stat 1024\n");
    
    printf("--------------------------------------------------\n");
    printf("ARGUMENT DESC:\n");
    printf("\t\t inpf - Input filename.\n");
    printf("\t\t outf - Output filename.\n");
    printf("\t\t init bucket size - Starting bucket size for hash table. "
            "Can be a number (power of two) or string 'guess' - program "
            "will try to guess based on file size and average word density.\n");
    
}

/**
 *  void run(int argc, char **argv)
 * 
 *  Runs whole program, reads command line arguments, opens input and output files, 
 *  initiates process_input and write_stats afterwards.
 */
void run(int argc, char **argv) {
    if(argc < 3 || argc > 4) {
        help();
        exit(1);
    }
    
    open_file(&input_file, argv[1], "rb");
    open_file(&output_file, argv[2], "wb");
    
    if(argc == 4) {
        if((strlen(argv[3]) == 5) && (strcmp(argv[3], "guess") == 0)) {
            printf("Guessing optimal hash table size...\n");
            hash_guess_count(get_file_size(input_file));
        }
        else if(get_str_number(argv[3]) > 0) {
            printf("Setting hash table size to %ld ...\n", get_str_number(argv[3]));
            hash_set_count(get_str_number(argv[3]));
        }
    }
    
    printf("Reading input file ...\n");
    
    process_input();
        
    printf("Saving stats to: %s ...\n", argv[2]);
    write_stats(output_file);
    
    printf("Exiting ...\n");
}

/**
 *  void close()
 * 
 *  Frees hash table and closes input and output file.
 */
void close() {
    stat_free();
    
    fclose(input_file);
    fclose(output_file);
}

/**
 *  int main(int argc, char **argv)
 * 
 *  Initiates and closes program.
 */
int main(int argc, char** argv) {
    run(argc, argv);
    close();
    
    return (EXIT_SUCCESS);
}
