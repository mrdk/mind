// mind -- a Forth interpreter
// Copyright 2011-2014 Markus Redeker <cep@ibp.de>
//
// Published under the GNU General Public License version 2 or any
// later version, at your choice. There is NO WARRANY, not at all. See
// the file "copying" for details.

#include "io.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

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
    if ((inf->input = (cell)fopen(name, "r"))) {
        errno = 0;
	inf->current = fgetc((FILE*)inf->input);
    }
}

void file_close(textfile_t *inf)
{
    if (!fclose((FILE*)inf->input))
        errno = 0;              // Reset errno if no error occured

    inf->input = 0;
    inf->current = EOF;
}

void file_get(textfile_t *inf)
{
    inf->current = fgetc((FILE*)inf->input);

    if (inf->current == '\n')
	inf->lineno++;
    else if (inf->current == EOF)
        file_close(inf);
}

void lines_open(lines_t *seq, char* path)
{
    seq->path = (cell)path;
    if ((seq->file = (cell)fopen(path, "r"))) {
        errno = 0;              // Reset errno if no error occured
        seq->line = 0;
        seq->lineno = 0;
        lines_get(seq);
    }
}

void lines_close(lines_t *seq)
{
    if (!fclose((FILE*)seq->file))
        errno = 0;              // Reset errno if no error occured

    seq->file = 0;
    seq->lineno++;

    if (seq->line) {
        free((void*)seq->line);
        seq->line = 0;
    }
}

void lines_get(lines_t *seq)
{
    char *lineptr = (char*)seq->line;
    size_t len;

    if (getline(&lineptr, &len, (FILE*)seq->file) != -1) {
        seq->line = (cell)lineptr;
        seq->lineno++;
    } else
        lines_close(seq);
}
