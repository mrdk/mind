#include <stdio.h>
#include <string.h>
#include <stddef.h>

typedef long int INT;
typedef unsigned long UINT;
typedef void* cell;		/* A Forth cell. We require that
				 * sizeof(cell) == sizeof(INT) */

// Forth truth values
#define TRUE 	(INT)(-1)
#define FALSE 	0
#define BOOL(n)	((cell)((n) ? TRUE : FALSE))

/* ---------------------------------------------------------------------- */
/* I/O */

struct file_t {
    FILE *input;		/* Input file */
    INT lineno;
    INT pageno;
};

static void open_file(struct file_t *inf, char *name)
{
    inf->input = fopen(name, "r");
    inf->lineno = 0;
    inf->pageno = 0;
}

static INT get_file_char(struct file_t *inf)
{
    INT cin = fgetc(inf->input);

    switch (cin) {
    case '\n': inf->lineno += 1; break;
    case '\f': inf->lineno = 0; inf->pageno += 1; break;
    }

    return cin;
}

static INT get_char_in(struct file_t *inf, char *charlist)
{
    INT cin;

    do {
	cin = get_file_char(inf);
    } while (strchr(charlist, cin));

    return cin;
}

static void get_char_notin(struct file_t *inf, char *charlist)
{
    INT cin;

    do {
	cin = get_file_char(inf);
    } while (!strchr(charlist, cin));
}

static char *read_while(struct file_t *inf, char *pos, char *charlist)
{
    INT cin;

    while ((cin = get_file_char(inf)) != EOF && !strchr(charlist, cin))
	*pos++ = cin;

    return pos;
}

/* Place a counted string at pos, consisting of chars in charlist. */
static char *read_string(struct file_t *inf, char *pos, char* charlist)
{
    char *start = pos + 1;
    char *end = read_while(inf, start, charlist);
    *pos = end - start;

    return end;
}

// Read the next word from INPUT and store it as counted string at
// POS. Return whether the string is empty
static INT parse(struct file_t *inf, char *pos)
{
    INT cin = get_char_in(inf, "\f\n\t ");

    if (cin == EOF) {
	*pos = 0;
	return FALSE;
    } else {
	char *start = pos + 1;
	char *end;

	*start = cin;
	end = read_while(inf, start + 1, "\f\n\t ");
	*pos = end - start;

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

#define NAMESIZE 13		/* size of kernel name field */
struct counted {
    char count;
    char str[NAMESIZE];
};

#define COUNTED(str) { sizeof(str) - 1, str }

struct entry {
    struct entry *lfa;	/* link field address */
    struct counted *name;
    char flags;
    cell *cfa;			/* code field address */
    cell *doer;
    cell body[];
};

#define ENTRY(a, field) \
	((struct entry*)((char*)(a) - offsetof(struct entry, field)))

#define NEW_ENTRY(label, wflags)					\
{   .lfa   = i_##label ? &dict[i_##label - 1].head : NULL,	\
    .flags = wflags,						\
    .name  = &dict[i_##label].cname,				\
    .cfa   = &&label,						\
    .doer = NULL }

typedef struct {
    struct counted cname;
    struct entry head;
} full_entry;

#define HEADINIT(label, wname, wflags)		\
{    .cname = COUNTED(wname),			\
     .head  = NEW_ENTRY(label, wflags) },

static struct entry *find_word(struct entry *e, char *name, char count)
{
    for (;;) {
	if (e->name->count == count && !memcmp(e->name->str, name, count))
	    return e;

	if (e->lfa)
	    e = e->lfa;
	else
	    return NULL;
    }
}

/* ---------------------------------------------------------------------- */
/* Memory */

#define MEMBYTES (64 * 1024)	/* Memory in bytes */
#define MEMCELLS (MEMBYTES / sizeof(cell)) /* Memory in cells */

#define PR(label)  (&dict[i_##label].head.cfa) /* Primitive */

/* System variables */
struct sys_t {
    cell *r0;			/* Start of the return stack */
    char *dp;			/* Dictionary pointer */
    cell *s0;			/* Start of the parameter stack */
    struct entry *latest;	/* Pointer to the latest definition */
    INT state;			/* Compiler state */
    cell wordq;		        /* Called if word not found (Name: Retro) */
    struct file_t inf;		/* Input file */
    cell mem[MEMCELLS];		/* The memory */
} sys;

static void init_sys(full_entry dict[])
{
    sys.r0 = sys.mem + 100;
    sys.dp = (char*)(sys.r0 + 1);
    sys.s0 = sys.mem + MEMCELLS - 0x10; /* Top of memory + safety space */
    sys.latest = &dict[num_words - 1].head;
    sys.state = 0;
    sys.wordq = PR(notfound);
    open_file(&sys.inf, "start.mind");
}

/* ---------------------------------------------------------------------- */
/* Stack manipulation */

/* Return stack, growing downwards */
#define RPUSH(x) (*--rp = (x))
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

static char *aligned(char *addr, unsigned align)
{
    /* Assertion: alignment is a power of 2 */
    return (char*)(((UINT)addr + align - 1) & ~(align - 1));
}

#define ALIGNED(ptr, type)  aligned((ptr), __alignof(type))
#define ALIGN(type)	    sys.dp = ALIGNED(sys.dp, type)

#define COMMA(val, type) \
    ALIGN(type), *(type*)sys.dp = (val), sys.dp += sizeof(type)

/* ---------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    cell *ip;			/* Instruction Pointer */
    cell *w;			/* Word Pointer */
    cell *rp;			/* Return Stack Pointer */
    cell *sp;			/* Stack Pointer */

    static full_entry dict[] = {	/* Dictionary */
#define E HEADINIT
#include "heads.c"
#undef E
    };

boot:
    init_sys(dict);
abort:
    sp = sys.s0;
quit:
    rp = sys.r0;
    {
	static cell interpreter[] = { PR(interpret), PR(branch), interpreter };
	ip = interpreter;
	goto next;
    }

bye:
    return 0;

// ---------------------------------------------------------------------------
// Inner interpreter

next:				/* Address Interpreter */
    w = *ip++; goto **w;


docol:				/* Runtime of ":" */
    RPUSH(ip); ip = ENTRY(w, cfa)->body; goto next;

dodefer:			/* Runtime of Defer */
    RPUSH(ip); w = *ENTRY(w, cfa)->body; goto **w;

dovar:				/* Runtime of Variable */
    PUSH(ENTRY(w, cfa)->body); goto next;

dodoes: //			Runtime for Create ... does>
    PUSH(ENTRY(w, cfa)->body); RPUSH(ip); ip = ENTRY(w, cfa)->doer; goto next;

docol_addr:   FUNC0(&&docol);
dodefer_addr: FUNC0(&&dodefer);
dovar_addr:   FUNC0(&&dovar);
dodoes_addr:  FUNC0(&&dodoes);


semi:	// ;;		    end of colon definition		[Name: Retro]
    ip = RPOP; goto next;

zero_semi:  // 0;  ( 0 -- | n -- n ) exit if TOS = 0		[Name: Retro]
    if (!TOS) {
	DROP(1);
	ip = RPOP;
    }
    goto next;

execute: // ( a -- )
    w = TOS; DROP(1); goto **w;

// ---------------------------------------------------------------------------
// Outer interpreter

interpret:
    {
	char *pos = sys.dp;

	if (!parse(&sys.inf, pos))
	    goto bye;

	{
	    struct entry *e = find_word(sys.latest, pos + 1, *pos);
	    if (e) {
		if (sys.state && !(e->flags & IMMEDIATE)) {
		    COMMA(&e->cfa, cell);
		    goto next;
		}
		else
		    w = (cell*)&e->cfa;
	    } else
		w = sys.wordq;

	    /* Execute word at w */
	    static cell endcode[] = { PR(semi) };
	    RPUSH(ip);
	    ip = endcode;
	    goto **w;
	}
    }

notfound: // Tell that the word at sys.dp could not be interpreted
    {
	printf("p%li:l%li: not found: %.*s\n",
	       sys.inf.pageno, sys.inf.lineno, *sys.dp, sys.dp + 1);
	fclose(sys.inf.input);
	goto abort;
    }

parentick: // (') ( "word" -- cfa | 0 )
    {
	struct entry *e;
	char *pos = sys.dp;

	EXTEND(1);
	if (parse(&sys.inf, pos) && (e = find_word(sys.latest, pos + 1, *pos)))
	    TOS = &e->cfa;
	else
	    TOS = 0;
	goto next;
    }

parenfind: // (find) ( addr n -- cfa t=found | f )
    {
	INT n = (INT)TOS;
	char *addr = NOS;

	struct entry *e = find_word(sys.latest, addr, n);
	if (e) {
	    NOS = &e->cfa; TOS = (cell)TRUE;
	} else {
	    DROP(1); TOS = FALSE;
	}
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
    sys.dp += (INT)TOS; DROP(1); goto next;

comma: // , ( n -- )
    COMMA(TOS, cell); DROP(1); goto next;

comma_quote: // ,"
    sys.dp = read_string(&sys.inf, sys.dp, "\f\""); goto next;

parse: // ( -- a )
    parse(&sys.inf, sys.dp); EXTEND(1); TOS = sys.dp; goto next;

entry_comma: // ( a c -- )	Compile an entry with the name A, code field C
    {
	struct entry *e;
	struct counted *name = NOS;

	ALIGN(struct entry);
	e = (struct entry*)sys.dp;
	sys.dp += sizeof(struct entry);

	e->lfa = sys.latest;
	e->name = name;
	e->flags = 0;
	e->cfa = TOS;
	e->doer = NULL;

	sys.latest = e;
	DROP(2);
	goto next;
    }

link_to:     FUNC1(&ENTRY(TOS, lfa)->cfa);       // link>  ( lfa -- cfa )
flags_fetch: FUNC1((INT)ENTRY(TOS, cfa)->flags); // flags@ ( cfa -- n )
flags_store:                                     // flags! ( n cfa -- )
    ENTRY(TOS, cfa)->flags = (INT)NOS; DROP(2); goto next;

to_name: FUNC1(ENTRY(TOS, cfa)->name);  // >name ( cfa -- 'name )
to_doer: FUNC1(&ENTRY(TOS, cfa)->doer); // >doer ( cfa -- 'doer )

num_immediate: FUNC0(IMMEDIATE); // #immediate

// ---------------------------------------------------------------------------
// Inline constants

branch:
    ip = *ip; goto next;

zbranch:			/* flag -- */
    if (TOS)
	ip++;			/* ignore */
    else
	ip = *ip;		/* jump */
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
pageno:   FUNC0(&sys.inf.pageno);

// ---------------------------------------------------------------------------
// Return stack

rdrop:
    RDROP; goto next;

rto: // >r ( n -- )
    RPUSH(TOS); DROP(1); goto next;

rfrom: // r> ( -- n )
    EXTEND(1); TOS = RPOP; goto next;

r: FUNC0(*rp);

inline_string: { // ( -- a )   Returns address of counted string following
		 //            in code.
	char *addr = *(char**)rp;

	*rp = ALIGNED(addr + 1 + *addr, cell);
	EXTEND(1);
	TOS = addr;
	goto next;
    }

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


spfetch: FUNC0(&NOS); // sp@

// ---------------------------------------------------------------------------
// Arithmetics

zero: FUNC0(0);
one:  FUNC0(1);
minus_one: FUNC0(-1);
two:  FUNC0(2);

oneplus:  FUNC1((cell)((INT)TOS + 1)); // 1+  ( n -- n+1 )
oneminus: FUNC1((cell)((INT)TOS - 1)); // 1-  ( n -- n-1 )

minus:  FUNC2((INT)NOS - (INT)TOS); // -
plus:   FUNC2((INT)NOS + (INT)TOS); // +
times:  FUNC2((INT)NOS * (INT)TOS); // *
divide: FUNC2((INT)NOS / (INT)TOS); // /
utimes: FUNC2((UINT)NOS * (UINT)TOS); // u*
or:     FUNC2((INT)NOS | (INT)TOS);

equal:      FUNC2(BOOL(NOS == TOS)); // =
zero_equal: FUNC1(BOOL(TOS == 0));   // 0=
zero_less:  FUNC1(BOOL((INT)TOS < 0));    // 0<
uless:      FUNC2(BOOL((UINT)NOS < (UINT)TOS)); // u<
ugreater:   FUNC2(BOOL((UINT)NOS > (UINT)TOS)); // u>

// ---------------------------------------------------------------------------
// Memory

fetch:  FUNC1(*(cell**)TOS);	    // @  ( a -- n )
cfetch: FUNC1((INT)(*(char*)TOS));  // c@ ( a -- n )

store: // ! ( n a -- )
    *(cell*)TOS = NOS; DROP(2); goto next;

cells:     FUNC1((INT)TOS * sizeof(cell));
cellplus:  FUNC1((char*)TOS + sizeof(cell)); // cell+
cellminus: FUNC1((char*)TOS - sizeof(cell)); // cell-

// ---------------------------------------------------------------------------
// Input/Output

count: // ( a -- a' # )
    EXTEND(1);
    TOS = (cell)(INT)(*(char*)NOS);
    NOS = (char*)NOS + 1;
    goto next;

cr:
    putchar('\n'); goto next;

emit: // ( c -- )
    putchar((INT)TOS); DROP(1); goto next;

type: // ( a # -- )
    {
	INT n = (INT)TOS;
	char *addr = NOS;

	DROP(2);

	while (n--)
	    putchar(*addr++);
	goto next;
    }

hdot: // h. ( n -- )		print hexadecimal
    printf("%lx ", (INT)TOS); DROP(1); goto next;

blank: FUNC0(' ');

// ---------------------------------------------------------------------------

dotparen: // .(
    read_string(&sys.inf, sys.dp, "\f)");
    printf("%.*s", *sys.dp, sys.dp + 1);
    goto next;
}
