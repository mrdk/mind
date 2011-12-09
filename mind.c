#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "types.h"

// Forth truth values
#define TRUE 	~(cell)0
#define FALSE 	0
#define BOOL(n)	((n) ? TRUE : FALSE)

/* ---------------------------------------------------------------------- */
/* I/O */

struct file_t {
    FILE *input;		/* Input file */
    cell lineno;
};

static void open_file(struct file_t *inf, char *name)
{
    inf->input = fopen(name, "r");
    inf->lineno = 1;
}

static int get_file_char(struct file_t *inf)
{
    int cin = fgetc(inf->input);

    switch (cin) {
    case '\n': inf->lineno += 1; break;
    }

    return cin;
}

static int get_char_in(struct file_t *inf, char *charlist)
{
    int cin;

    do {
	cin = get_file_char(inf);
    } while (strchr(charlist, cin));

    return cin;
}

static void get_char_notin(struct file_t *inf, char *charlist)
{
    int cin;

    do {
	cin = get_file_char(inf);
    } while (!strchr(charlist, cin));
}

/* Place a string at pos, consisting of chars in charlist. */
static char *read_string(struct file_t *inf, char *pos, char *charlist)
{
    int cin;

    while ((cin = get_file_char(inf)) != EOF && !strchr(charlist, cin))
	*pos++ = cin;
    *pos++ = '\0';

    return pos;
}

// Read the next word from INPUT and store it as string at
// POS. Return whether the string is empty
static int parse(struct file_t *inf, char *pos)
{
    int cin = get_char_in(inf, "\f\n\t ");

    if (cin == EOF)
	return FALSE;
    else {
	char *start = pos;
	char *end;
    
	*start = cin;
	end = read_string(inf, start + 1, "\f\n\t ");

	return TRUE;
    }
}

/* ---------------------------------------------------------------------- */
/* Dictionary structure */

enum wordnum {
#define E(label, name, flags)  i_##label,
#include "heads.c"
#undef E
    num_words };

/* Interpreter flags */
#define IMMEDIATE 1

typedef void *label_t;		/* Target of computed goto. */

struct entry {
    cell lfa;	       /* link field address: points to next entry. */
    cell name;			/* Pointer to start of word name */
    char flags;
    cell cfa;			/* code field address */
    cell doer;	     /* Pointer to C routine that executes the word. */
    cell body[];
};

#define ENTRY(a, field) \
    ((struct entry*)((char*)(a) - offsetof(struct entry, field)))

#define NEW_ENTRY(label, wname, wflags)				\
{   .lfa   = (cell)(i_##label ? &dict[i_##label - 1] : NULL),	\
    .flags = wflags,						\
    .name  = (cell)wname,					\
    .cfa   = (cell)&&label,					\
    .doer  = (cell)NULL },

static struct entry *find_word(struct entry *e, char *name)
{
    for (;;) {
	if (!strcmp((char*)e->name, name))
	    return e;

	if (e->lfa)
	    e = (struct entry*)e->lfa;
	else
	    return NULL;
    }
}

/* ---------------------------------------------------------------------- */
/* Memory */

#define MEMBYTES (64 * 1024)	/* Memory in bytes */
#define MEMCELLS (MEMBYTES / sizeof(cell)) /* Memory in cells */

#define PR(label)  ((cell)(&dict[i_##label].cfa)) /* Primitive */

/* System variables */
struct sys_t {
    cell r0;			/* Pointer: Start of the return stack */
    cell dp;			/* Dictionary pointer */
    cell s0;			/* Pointer: Start of the parameter stack */
    cell latest;	  /* (struct entry*): The latest definition */
    cell state;			/* Compiler state */
    cell wordq;		  /* Called if word not found (Name: Retro) */
    struct file_t inf;		/* Input file */
    cell mem[MEMCELLS];		/* The memory */
} sys;

static void init_sys(struct entry dict[])
{
    sys.r0 = (cell)(sys.mem + 100);
    sys.dp = sys.r0 + sizeof(cell);
    sys.s0 = (cell)(sys.mem + MEMCELLS - 0x10); /* Top of memory + safety space */
    sys.latest = (cell)&dict[num_words - 1];
    sys.state = 0;
    sys.wordq = PR(notfound);
    open_file(&sys.inf, "start.mind");
}

/* ---------------------------------------------------------------------- */
/* Stack manipulation */

/* Return stack, growing downwards */
#define RPUSH(x) (*--rp = (cell)(x))
#define RPOP     (*rp++)
#define RDROP    (rp++)

/* Parameter stack, growing upwards */
#define EXTEND(n)   	sp += (n) /* Extend the stack but don't initialise */
#define DROP(n)		sp -= (n)
#define PUSH(x) 	EXTEND(1), TOS = (cell)(x)

#define TOS	(*sp)
#define NOS     (*(sp - 1))
#define ST(n)   (*(sp - (n)))

// Macros for "functions": words with 1 parameter a result
#define FUNC0(x)  EXTEND(1); TOS = (cell)(x); goto next // ( -- n )
#define FUNC1(x)  TOS = (cell)(x); goto next            // ( n1 -- n2 )
#define FUNC2(x)  NOS = (cell)(x); DROP(1); goto next   // ( n1 n2 -- n3 )

/* ---------------------------------------------------------------------- */
/* Code and dictionary */

static cell aligned(cell addr, cell align)
{
    /* Assertion: alignment is a power of 2 */
    return (addr + align - 1) & ~(align - 1);
}

#define ALIGNED(ptr, type)  aligned((ptr), __alignof(type))
#define ALIGN(type)	    sys.dp = ALIGNED(sys.dp, type)

#define COMMA(val, type) \
    ALIGN(type), *(type*)sys.dp = (type)(val), sys.dp += sizeof(type)

/* ---------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    cell *ip;			/* Instruction Pointer */
    label_t *w;			/* Word Pointer */
    cell *rp;			/* Return Stack Pointer */
    cell *sp;			/* Stack Pointer */

    static struct entry dict[] = { /* Dictionary */
#define E NEW_ENTRY
#include "heads.c"
#undef E
    };

// ---------------------------------------------------------------------------
// Starting and ending
boot:
    init_sys(dict);
abort:
    sp = (cell*)sys.s0;
quit:
    rp = (cell*)sys.r0;
    {
	static cell interpreter[] =
	    { PR(interpret), PR(branch), (cell)interpreter };
	ip = interpreter;
	goto next;
    }

bye:
    return 0;

// ---------------------------------------------------------------------------
// Inner interpreter

next:				/* Address Interpreter */
    w = (label_t*)*ip++; goto **w;

docol:				/* Runtime of ":" */
    RPUSH(ip); ip = ENTRY(w, cfa)->body; goto next;

dodefer:			/* Runtime of Defer */
    RPUSH(ip); w = (label_t)*ENTRY(w, cfa)->body; goto **w;

dovar:				/* Runtime of Variable */
    PUSH(ENTRY(w, cfa)->body); goto next;

dodoes: //			Runtime for Create ... does>
    PUSH(ENTRY(w, cfa)->body);
    RPUSH(ip); ip = (cell*)ENTRY(w, cfa)->doer; goto next;

docol_addr:   FUNC0(&&docol);
dodefer_addr: FUNC0(&&dodefer);
dovar_addr:   FUNC0(&&dovar);
dodoes_addr:  FUNC0(&&dodoes);


semi:	// ;;		    end of colon definition		[Name: Retro]
    ip = (cell*)RPOP; goto next;

zero_semi:  // 0;  ( 0 -- | n -- n ) exit if TOS = 0		[Name: Retro]
    if (!TOS) {
	DROP(1);
	ip = (cell*)RPOP;
    }
    goto next;

execute: // ( a -- )
    w = (label_t*)TOS; DROP(1); goto **w;

// ---------------------------------------------------------------------------
// Outer interpreter

interpret:
    {
	char *pos = (char*)sys.dp;

	if (!parse(&sys.inf, pos))
	    goto bye;

	{
	    struct entry *e = find_word((struct entry*)sys.latest, pos);
	    if (e) {
		if (sys.state && !(e->flags & IMMEDIATE)) {
		    COMMA(&e->cfa, cell);
		    goto next;
		}
		else
		    w = (label_t*)&e->cfa;
	    } else
		w = (label_t*)sys.wordq;

	    /* Execute word at w */
	    static cell endcode[] = { PR(semi) };
	    RPUSH(ip);
	    ip = endcode;
	    goto **w;
	}
    }

notfound: // Tell that the word at sys.dp could not be interpreted
    {
	printf("l%"PRIdCELL": not found: %s\n",
	       sys.inf.lineno, (char*)sys.dp);
	fclose(sys.inf.input);
	goto abort;
    }

parentick: // (') ( "word" -- cfa | 0 )
    {
	struct entry *e;
	char *pos = (char*)sys.dp;

	EXTEND(1);
	if (parse(&sys.inf, pos) &&
	    (e = find_word((struct entry*)sys.latest, pos)))
	    TOS = (cell)&e->cfa;
	else
	    TOS = 0;
	goto next;
    }

parenfind: // (find) ( addr -- cfa | 0 )
    {
	char *addr = (char*)TOS;

	struct entry *e = find_word((struct entry*)sys.latest, addr);
	if (e)
	    TOS = (cell)&e->cfa;
	else
	    TOS = 0;
	goto next;
    }

lbrack:	// [
    sys.state = 0; goto next;
rbrack:	// ]
    sys.state = 1; goto next;

backslash: // "\": comment to the end of the line
    get_char_notin(&sys.inf, "\f\n"); goto next;

paren: // "("
    get_char_notin(&sys.inf, "\f)"); goto next;

// ---------------------------------------------------------------------------
// Dictionary

align:
    ALIGN(cell); goto next;

allot: // ( n -- )
    sys.dp += TOS; DROP(1); goto next;

comma: // , ( n -- )
    COMMA(TOS, cell); DROP(1); goto next;

ccomma: // c, ( n -- )
    COMMA(TOS, char); DROP(1); goto next;

comma_quote: // ,"
    sys.dp = (cell)read_string(&sys.inf, (char*)sys.dp, "\f\""); goto next;

parse: // ( -- a )
    parse(&sys.inf, (char*)sys.dp); EXTEND(1); TOS = sys.dp; goto next;

entry_comma: // ( a c -- )	Compile an entry with the name A, code field C
    {
	struct entry *e;

	ALIGN(struct entry);
	e = (struct entry*)sys.dp;
	sys.dp += sizeof(struct entry);

	e->lfa = sys.latest;
	e->name = NOS;
	e->flags = 0;
	e->cfa = TOS;
	e->doer = (cell)NULL;

	sys.latest = (cell)e;
	DROP(2);
	goto next;
    }

link_to:     FUNC1(&ENTRY(TOS, lfa)->cfa);       // link>  ( lfa -- cfa )
flags_fetch: FUNC1(ENTRY(TOS, cfa)->flags); // flags@ ( cfa -- n )
flags_store:                                     // flags! ( n cfa -- )
    ENTRY(TOS, cfa)->flags = NOS; DROP(2); goto next;

to_name: FUNC1(ENTRY(TOS, cfa)->name);	// >name ( cfa -- 'name )
to_doer: FUNC1(&ENTRY(TOS, cfa)->doer); // >doer ( cfa -- 'doer )

num_immediate: FUNC0(IMMEDIATE); // #immediate

// ---------------------------------------------------------------------------
// Inline constants

branch:
    ip = (cell*)*ip; goto next;

zbranch:			/* flag -- */
    if (TOS)
	ip++;			/* ignore */
    else
	ip = (cell*)*ip;	/* jump */
    DROP(1);
    goto next;

lit:				/* -- n */
    PUSH(*ip++); goto next;

// ---------------------------------------------------------------------------
// System variables

s0:       FUNC0(&sys.s0);
r0:       FUNC0(&sys.r0);
latest:   FUNC0(&sys.latest);
dp:       FUNC0(&sys.dp);
here:     FUNC0(sys.dp);
state:    FUNC0(&sys.state);
wordq:    FUNC0(&sys.wordq);
lineno:   FUNC0(&sys.inf.lineno);

// ---------------------------------------------------------------------------
// Return stack

rdrop:
    RDROP; goto next;

rto: // >r ( n -- )
    RPUSH(TOS); DROP(1); goto next;

rfrom: // r> ( -- n )
    EXTEND(1); TOS = RPOP; goto next;

r: FUNC0(*rp);

// ---------------------------------------------------------------------------
// Stack

drop: // ( n -- )
    DROP(1); goto next;

nip: // ( n1 n2 -- n2 )
    NOS = TOS; DROP(1); goto next;

twodrop: // 2drop ( n1 n2 -- )
    DROP(2); goto next;

qdup: // ?dup ( 0 -- 0 | n -- n n  if n > 0)
    if (TOS == 0)
	goto next;		/* ...else continue with DUP */
dup: // ( n -- n n )
    EXTEND(1); TOS = NOS; goto next;

twodup: // a b -- a b a b
    EXTEND(2); NOS = ST(3); TOS = ST(2); goto next;

over: // ( a b -- a b a )
    EXTEND(1); TOS = ST(2); goto next;
under: // ( a b -- b a b )
    { cell a = NOS, b = TOS; EXTEND(1); ST(2) = TOS = b; NOS = a; goto next; }

swap: // ( a b -- b a )
    { cell tmp = TOS; TOS = NOS; NOS = tmp; goto next; }

rot: // ( a b c -- b c a )
    { cell tmp = ST(2); ST(2) = NOS; NOS = TOS; TOS = tmp; goto next; }
minus_rot: // ( a b c -- c a b )
    { cell tmp = TOS; TOS = NOS; NOS = ST(2); ST(2) = tmp; goto next; }


spfetch: FUNC0(&NOS); // sp@ ( -- addr )

spstore: // sp! ( addr -- )
    sp = (cell*)TOS; DROP(1); goto next;

// ---------------------------------------------------------------------------
// Arithmetics

zero: FUNC0(0);
one:  FUNC0(1);
minus_one: FUNC0(-1);
two:  FUNC0(2);

oneplus:  FUNC1(TOS + 1); // 1+  ( n -- n+1 )
oneminus: FUNC1(TOS - 1); // 1-  ( n -- n-1 )

minus:  FUNC2(NOS - TOS); // -
plus:   FUNC2(NOS + TOS); // +
times:  FUNC2(NOS * TOS); // *
divide: FUNC2(NOS / TOS); // /
mod: 	FUNC2(NOS % TOS); // mod
utimes: FUNC2((ucell)NOS * (ucell)TOS); // u*
udivide: FUNC2((ucell)NOS / (ucell)TOS); // u/
abs:    FUNC1(cellabs(TOS));
or:     FUNC2(NOS | TOS);
and:    FUNC2(NOS & TOS);
xor:    FUNC2(NOS ^ TOS);
invert:	FUNC1(~TOS);

divmod: // /mod ( n1 n2 -- div mod )
    { 
	celldiv_t res = celldiv(NOS, TOS);
	NOS = res.quot;
	TOS = res.rem;
	goto next;
    }

equal:      FUNC2(BOOL(NOS == TOS)); // =
unequal:    FUNC2(BOOL(NOS != TOS)); // <>
zero_equal: FUNC1(BOOL(TOS == 0));   // 0=
zero_less:  FUNC1(BOOL(TOS < 0));    // 0<
zero_greater: FUNC1(BOOL(TOS > 0));  // 0>
less:       FUNC2(BOOL(NOS < TOS)); // <
less_eq:    FUNC2(BOOL(NOS <= TOS)); // <=
greater:    FUNC2(BOOL(NOS > TOS)); // >
greater_eq: FUNC2(BOOL(NOS >= TOS)); // >=
uless:      FUNC2(BOOL((ucell)NOS <  (ucell)TOS)); // u<
uless_eq:   FUNC2(BOOL((ucell)NOS <= (ucell)TOS)); // u<=
ugreater:   FUNC2(BOOL((ucell)NOS >   (ucell)TOS)); // u>
ugreater_eq: FUNC2(BOOL((ucell)NOS >= (ucell)TOS)); // u>=

// ---------------------------------------------------------------------------
// Memory

fetch:  FUNC1(*(cell*)TOS);	// @  ( a -- n )
cfetch: FUNC1(*(char*)TOS);	// c@ ( a -- n )

store: // ! ( n a -- )
    *(cell*)TOS = NOS; DROP(2); goto next;

cstore: // c! ( n a -- )
    *(char*)TOS = NOS; DROP(2); goto next;

malloc: FUNC1(malloc(TOS)); // ( n -- addr )
free:                       // ( addr -- )
    free((void*)TOS); DROP(1); goto next;

cells:     FUNC1(TOS * sizeof(cell));
cellplus:  FUNC1(TOS + sizeof(cell)); // cell+
cellminus: FUNC1(TOS - sizeof(cell)); // cell-

// ---------------------------------------------------------------------------
// Input/Output

strlen: FUNC1(strlen((char*)TOS)); // ( a -- # )

cr:
    putchar('\n'); goto next;

emit: // ( c -- )
    putchar(TOS); DROP(1); goto next;

type: // ( a # -- )
    {
	cell n = TOS;
	char *addr = (char*)NOS;

	DROP(2);

	while (n--)
	    putchar(*addr++);
	goto next;
    }

puts: // ( a -- )               print null-terminated string
    fputs((char*)TOS, stdout); DROP(1); goto next;

hdot: // h. ( n -- )		print hexadecimal
    printf("%"PRIxCELL" ", TOS); DROP(1); goto next;

blank: FUNC0(' ');

// ---------------------------------------------------------------------------
// Others

dotparen: // .(
    read_string(&sys.inf, (char*)sys.dp, "\f)");
    printf("%s", (char*)sys.dp);
    goto next;
}
