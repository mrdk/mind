// mind -- a Forth interpreter
// Copyright 2011-2013 Markus Redeker <cep@ibp.de>
//
// Published under the GNU General Public License version 2 or any
// later version, at your choice. There is NO WARRANY, not at all. See
// the file "copying" for details.

// This file contains code to handle the command line parameters.

#ifndef ARGS_H
#define ARGS_H

#include "types.h"

typedef struct {
    cell *raw_argv;             // (char**) content of argv
    cell raw_argc;              // (int) argc
    cell *argv;                 // (char**) content of argv
    cell argc;                  // (int) argc
    cell progname;              // (char*) argv[0]
    cell command;		// (char*) command parameter
    cell interactive;		// flag: start the interactive mode
} args_t;

extern args_t args;

void init_args(int argc, char *argv[]);

#endif
