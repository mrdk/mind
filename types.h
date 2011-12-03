#ifndef TYPES_H
#define TYPES_H

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

/* Define the type cell as an integer type large enough to contain
 * both an integer and a pointer.

 * Also, if libc provides different versions of a function dependent
 * on the type of integer chosen, we define our own version of it. */

#if INTPTR_MAX <= INT_MAX	/* cell == int */
typedef int cell;
typedef unsigned int UINT;

typedef div_t celldiv_t;
#define celldiv div

#elif INTPTR_MAX <= LONG_MAX	/* cell == long int */
typedef long int cell;
typedef unsigned long int UINT;

typedef ldiv_t celldiv_t;
#define celldiv ldiv

#else  /* cell == long long int? Maybe we won't ever need it.  */
#error sizeof(void*) > sizeof(long int): not yet supported.
#endif

#endif
