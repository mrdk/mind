// mind -- a Forth interpreter
// Copyright 2011-2014 Markus Redeker <cep@ibp.de>
//
// Published under the GNU General Public License version 2 or any
// later version, at your choice. There is NO WARRANY, not at all. See
// the file "copying" for details.

// This file contains code to handle the command line parameters.

#include "args.h"

#include <stdio.h>
#include <unistd.h>

args_t args;

static void usage(char *progname)
{
    printf("Usage: %s [-e cmd | -x cmd | -h ]\n", progname);
    printf("Options and arguments:\n");
    printf("-e cmd: Execute cmd and then stop\n");
    printf("-x cmd: Execute cmd, then start command prompt.\n");
    printf("-h    : Print this help text\n");
}

// Create a copy of ARGV as a forth-style array.
static void copy_args(int argc, char *argv[])
{
    int i;

    args.raw_argc = argc;
    args.raw_argv = malloc((argc + 1) * sizeof(argv));

    for (i = 0; i <= argc; i++)
        args.raw_argv[i] = (cell)argv[i];
}

void init_args(int argc, char *argv[])
{
    copy_args(argc, argv);

    // Default: start in interactive mode
    args.command = 0;
    args.interactive = TRUE;

    int opt;
    while ((opt = getopt(argc, argv, "he:x:")) != -1) {
	switch (opt) {
	case 'h':
            usage(argv[0]);
	    exit(0);
	case 'e':
	    args.command = (cell)optarg;
	    args.interactive = FALSE;
	    break;
	case 'x':
	    args.command = (cell)optarg;
	    args.interactive = TRUE;
	    break;
	default:
	    exit(-1);
	}
    }

    // The words `argv` and `argc` then contain the remaining
    // arguments, to be processed by the program.
    args.argc = argc - optind;
    args.argv = args.raw_argv + optind;
}
