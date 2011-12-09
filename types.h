#ifndef TYPES_H
#define TYPES_H

#include <limits.h>
//#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

/* Define the type cell as an integer type large enough to contain
 * both an integer and a pointer.

 * Also, if libc provides different versions of a function dependent
 * on the type of integer chosen, we define our own version of it. */

#if INTPTR_MAX <= INT_MAX	/* cell == int */
typedef int cell;
typedef unsigned int ucell;

typedef div_t celldiv_t;
#define celldiv div

#define PRIdCELL  "d"		/* Print format specifier: decimal */
#define PRIxCELL  "x"		/* Print format specifier: hex */

#elif INTPTR_MAX <= LONG_MAX	/* cell == long int */
typedef long int cell;
typedef unsigned long int ucell;

typedef ldiv_t celldiv_t;
#define celldiv ldiv

#define PRIdCELL  "ld"		/* Print format specifier: decimal */
#define PRIxCELL  "lx"		/* Print format specifier: hex */

#else  /* cell == long long int? Maybe we won't ever need it.  */
#error sizeof(void*) > sizeof(long int): not yet supported.
#endif

#endif
