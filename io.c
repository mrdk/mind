// mind -- a Forth interpreter
// Copyright 2011-2013 Markus Redeker <cep@ibp.de>
//
// Published under the GNU General Public License version 2 or any
// later version, at your choice. There is NO WARRANY, not at all. See
// the file "copying" for details.

#include "io.h"

#include <stdio.h>

void open_textfile(textfile_t *inf, char* name)
{
    inf->name = (cell)name;
    if ((inf->input = (cell)fopen(name, "r")))
	inf->current = fgetc((FILE*)inf->input);
}

void close_textfile(textfile_t *inf)
{
    fclose((FILE*)inf->input);

    inf->input = 0;
    inf->current = EOF;
}

void file_forward(textfile_t *inf)
{
    inf->current = fgetc((FILE*)inf->input);

    if (inf->current == '\n')
	inf->lineno++;
}