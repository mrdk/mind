// mind -- a Forth interpreter
// Copyright 2011-2013 Markus Redeker <cep@ibp.de>
//
// Published under the GNU General Public License version 2 or any
// later version, at your choice. There is NO WARRANY, not at all. See
// the file "copying" for details.

#include "io.h"

#include <stdio.h>
#include <string.h>

// Interpret FILENAME relative to the directory of MIND_FILE.
char *mind_relative(char* mind_file, char *filename)
{
    char mind_dir[PATH_MAX];
    char *slash = strrchr(mind_file, '/');
    if (slash)
	strncpy(mind_dir, mind_file, slash - mind_file);
    else
	strcpy(mind_dir, mind_file);

    char *path = malloc(PATH_MAX);
    sprintf(path, "%s/%s", mind_dir, filename);
    return path;
}

void file_open(textfile_t *inf, char* name)
{
    inf->name = (cell)name;
    if ((inf->input = (cell)fopen(name, "r")))
	inf->current = fgetc((FILE*)inf->input);
}

void file_close(textfile_t *inf)
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
