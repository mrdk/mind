// mind -- a Forth interpreter
// Copyright 2011-2013 Markus Redeker <cep@ibp.de>
//
// Published under the GNU General Public License version 2 or any
// later version, at your choice. There is NO WARRANY, not at all. See
// the file "copying" for details.

#ifndef IO_H
#define IO_H

#include "types.h"

typedef struct {
    cell forward;		// Forth word ( stream -- )
    cell current_fetch;		// Forth word ( stream -- char )
    cell validq;                // Forth word ( stream -- flag )
} stream_t;

typedef struct {
    stream_t stream;
    cell input;			// (FILE*) Input file
    cell name;                  // (char*) File name
    cell current;		// Character at input position (or EOF)
    cell lineno;		// integer: line number
    cell caller;                // (textfile_t*) File that called it
} textfile_t;

char *mind_relative(char *mind_file, char *filename);

void file_open(textfile_t *inf, char* name);
void file_close(textfile_t *inf);
void file_forward(textfile_t *inf);

#endif
