// mind -- a Forth interpreter
// Copyright 2011-2013 Markus Redeker <cep@ibp.de>
//
// Published under the GNU General Public License version 2 or any
// later version, at your choice. There is NO WARRANY, not at all. See
// the file "copying" for details.

#include "types.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#define MEMCELLS 0x10000	/* Memory size in cells */

// Forth truth values
#define TRUE 	~(cell)0
#define FALSE 	0
#define BOOL(n)	((n) ? TRUE : FALSE)

/* ---------------------------------------------------------------------- */
/* Dictionary structure */

// Enumerate the dictionary entries. The word `foo` gets the number
// `i_foo`, and its dictionary entry is found at dict[i_foo].
enum wordnum_t {
#define E(label, ...)  i_##label,
#define D E
#include "heads.c"
#undef D
#undef E
    num_words };

/* Interpreter flags */
#define IMMEDIATE 1

typedef void *label_t;		/* Target of computed goto. */

typedef struct {
    cell link;      // (entry*)  Pointer to previous entry.
    cell name;      // (char*)   Pointer to start of word name.
    char flags;
    cell exec;      // (label_t) C code that executes this word.
    cell doer;      // (cell*)   Forth routine for `does>` part.
    cell body[];
} entry_t;

#define NEW_WORD(label, wname, wflags)				\
{   .link  = i_##label ? (cell)&dict[i_##label - 1] : 0,	\
    .flags = wflags,						\
    .name  = (cell)wname,					\
    .exec = (cell)&&label,					\
    .doer  = 0 },

#define NEW_DEFER(label, wname, wdoer, wflags)                  \
{   .link  = i_##label ? (cell)&dict[i_##label - 1] : 0,	\
    .flags = wflags,						\
    .name  = (cell)wname,					\
    .exec = (cell)&&dodefer,					\
    .doer  = C(wdoer) },

// Compute the address of an entry_t field when given an execution
// token instead of the beginning of the struct.
#define FROM_XT(xt)                                             \
    ((entry_t*)((char*)(xt) - offsetof(entry_t, exec)))

/* Find XT for *name* in dictionary, starting at *e*. */
static cell* find_xt(entry_t *e, char *name)
{
    for (; e; e = (entry_t*)e->link) {
	if (!strcmp((char*)e->name, name))
	    return &e->exec;
    }
    return NULL;
}

typedef struct {
    cell link;                  // (context_t*) Previous context in chain
    cell last;                  // (entry_t*)   The last definition
    cell find_word;             // Forth word ( str ctx -- xt | 0 )
} context_t;

/* ----------------------------------------------------------------------- */
/* Define hand-compiled Forth code */
#define CODE(...)					\
    {							\
	static cell start[] = {__VA_ARGS__, C(semi) };	\
	RPUSH(ip), ip = start; goto next;		\
    }

#define C(label)  ((cell)(&dict[i_##label].exec)) // Call to Forth word

// ---------------------------------------------------------------------------
// Reading text files and interactive input

typedef struct {
    cell forward;		// Forth word ( stream -- )
    cell current_fetch;		// Forth word ( stream -- char )
    cell eos;			// Forth word ( stream -- flag )
} stream_t;

typedef struct {
    stream_t stream;
    cell input;			// (FILE*) Input file
    cell name;                  // (char*) File name
    cell current;		// Character at input position (or EOF)
    cell lineno;		// integer: line number
} textfile_t;

static void init_textfile(textfile_t *inf, entry_t dict[])
{
    inf->stream.forward = C(file_forward);
    inf->stream.current_fetch = C(file_current_fetch);
    inf->stream.eos = C(file_eof);
    inf->input = 0;
    inf->name = 0;
    inf->current = EOF;
}

static void open_textfile(textfile_t *inf, char* name)
{
    inf->name = (cell)name;
    if ((inf->input = (cell)fopen(name, "r")))
	inf->current = fgetc((FILE*)inf->input);
}

static void close_textfile(textfile_t *inf)
{
    fclose((FILE*)inf->input);

    inf->input = 0;
    inf->current = EOF;
}

static void file_forward(textfile_t *inf)
{
    inf->current = fgetc((FILE*)inf->input);

    if (inf->current == '\n')
	inf->lineno++;
}

// ---------------------------------------------------------------------------
// Memory

// System variables
struct {
    cell r0;		     // (cell*) Start of the return stack
    cell dp;		     // (cell*) Dictionary pointer
    cell s0;		     // (cell*) Start of the parameter stack
    cell state;		     // Compiler state
    cell wordq;		     // Called if word not found
    context_t root;          // root context
    textfile_t inf;	     // Input file
    cell instream;	     // (stream_t*) Current input stream
    cell mem[MEMCELLS];	     // The memory
} sys;

static void init_sys(entry_t dict[])
{
    sys.r0 = (cell)(sys.mem + 100);
    sys.dp = sys.r0 + sizeof(cell);
    sys.s0 = (cell)(sys.mem + MEMCELLS - 0x10); // Top of memory + safety space
    sys.state = 0;
    sys.wordq = C(notfound);
    sys.root.link = 0;
    sys.root.last = (cell)&dict[num_words - 1];
    sys.root.find_word = C(find_word);
    init_textfile(&sys.inf, dict);
    sys.instream = (cell)&sys.inf.stream;
}

static struct {			// Program arguments:
    cell command;		// (char*) command parameter
    cell interactive;		// flag: start the interactive mode
} args;

/* ---------------------------------------------------------------------- */
/* Stack manipulation */

/* Return stack, growing downwards */
#define RPUSH(x) (*--rp = (cell)(x))
#define RPOP     (*rp++)
#define RDROP    (rp++)

/* Parameter stack, growing downwards */
#define EXTEND(n)   	sp -= (n) /* Extend the stack but don't initialise */
#define DROP(n)		sp += (n)
#define PUSH(x) 	EXTEND(1), TOS = (cell)(x)

// Stack positions
#define TOS	(*sp)		// Top of Stack
#define NOS     sp[1]		// Next in Stack

// Macros for "functions": words with 1 parameter a result
#define FUNC0(x)  EXTEND(1); TOS = (cell)(x); goto next // ( -- n )
#define FUNC1(x)  TOS = (cell)(x); goto next            // ( n1 -- n2 )
#define FUNC2(x)  NOS = (cell)(x); DROP(1); goto next   // ( n1 n2 -- n3 )

// Macro for a word that computes the address of a field from the
// address of a struct
#define OFFSET(type, field)   FUNC1(TOS + offsetof(type, field))

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

void mind()
{
    cell *ip;			/* Instruction Pointer */
    label_t *w;			/* Word Pointer */
    cell *rp;			/* Return Stack Pointer */
    cell *sp;			/* Stack Pointer */

    static entry_t dict[] = { /* Dictionary */
#define E NEW_WORD
#define D NEW_DEFER
#include "heads.c"
#undef D
#undef E
    };

// ---------------------------------------------------------------------------
// Starting and ending
boot:
    init_sys(dict);

    open_textfile(&sys.inf, "init.mind"); 
    if (sys.inf.current == EOF) {
	fprintf(stderr, "Error: File '%s' not found\n", (char*)sys.inf.name);
	exit(-1);
    }

    sp = (cell*)sys.s0;
    rp = (cell*)sys.r0;
    {
	static cell interpreter[] = { C(do_stream), C(abort) };
	ip = interpreter;
	goto next;
    }

bye:
    return;

// ---------------------------------------------------------------------------
// Inner interpreter

next:				/* Address Interpreter */
    w = (label_t*)*ip++; goto **w;

docol:				/* Runtime of ":" */
    RPUSH(ip); ip = FROM_XT(w)->body; goto next;

dodefer:			/* Runtime of Defer */
    w = (label_t*)FROM_XT(w)->doer; goto **w;

dovar:				/* Runtime of Variable */
    PUSH(FROM_XT(w)->body); goto next;

dodoes: //			Runtime for Create ... does>
    PUSH(FROM_XT(w)->body);
    RPUSH(ip); ip = (cell*)FROM_XT(w)->doer; goto next;

docol_addr:   FUNC0(&&docol);
dodefer_addr: FUNC0(&&dodefer);
dovar_addr:   FUNC0(&&dovar);
dodoes_addr:  FUNC0(&&dodoes);


semi:	// ;;		    end of colon definition
    ip = (cell*)RPOP; goto next;

if_semi: // if; ( n -- )  exit if TOS <> 0
    if (*sp++) {
	ip = (cell*)RPOP;
    }
    goto next;

zero_semi:  // 0;  ( 0 -- | n -- n ) exit if TOS = 0
    if (!TOS) {
	DROP(1);
	ip = (cell*)RPOP;
    }
    goto next;

execute: // ( a -- )
    w = (label_t*)TOS; DROP(1); goto **w;

// ---------------------------------------------------------------------------
// Outer interpreter

exec_compile: // exec/compile ( xt -- )
    {
        cell xt = TOS;
	DROP(1);

	if (xt) {
            entry_t *e = FROM_XT(xt);

	    if (sys.state && !(e->flags & IMMEDIATE)) {
		COMMA(&e->exec, cell);
		goto next;
	    }
	    else
		w = (label_t*)&e->exec;
	} else
	    w = (label_t*)sys.wordq;

	goto **w;
    }

interpret:  // : interpret   parse find exec/compile ;
    CODE(C(parse), C(find), C(exec_compile));

notfound: // Tell that the word at sys.dp could not be interpreted
    {
	printf("l%"PRIdCELL": not found: %s\n",
	       ((textfile_t*)sys.instream)->lineno, (char*)sys.dp);
        close_textfile(&sys.inf);
        w = (label_t)C(abort); goto **w;
    }

find: // find ( str -- xt | 0 )
    CODE(C(lit), (cell)&sys.root, C(find_word));

find_word: // find-word ( str ctx -- xt | 0 )
    FUNC2(find_xt((entry_t*)((context_t*)TOS)->last, (char*)NOS));

lbrack:	// [
    sys.state = 0; goto next;
rbrack:	// ]
    sys.state = 1; goto next;

parse_to: // : parse-to ( addr string -- )
          //   >r BEGIN current@ append  forward
          //            r@ current@ strchr  eos or UNTIL rdrop
          //      0 over c!  eos if; forward ;
    CODE(C(rto),
	 C(current_fetch), C(append), C(forward),
	 C(rfetch), C(current_fetch), C(strchr), C(eos), C(or),
	 C(zbranch), (cell)(start + 1), C(rdrop),
	 C(zero), C(swap), C(cstore),
         C(eos), C(if_semi), C(forward));

skip_whitespace: // : skip-whitespace ( -- )
	         //   BEGIN  whitespace current@ strchr 0= if;  forward AGAIN ;
// : whitespace-eos  ( -- ? )
//   whitespace current@ strchr 0=  eos or ;
    CODE(C(whitespace), C(current_fetch), C(strchr), C(zero_equal),
	 C(if_semi), C(forward), C(branch), (cell)(start));

parse: // : parse ( -- addr )
       //   skip-whitespace  here whitespace parse-to  here ;
    CODE(C(skip_whitespace), C(here), C(whitespace), C(parse_to), C(here));

backslash: // : \   BEGIN current@ forward  #eol = if;  eos UNTIL ; immediate
// : backslash-eos  ( -- ? )   current@ #eol =  eos or ;
    CODE(C(current_fetch), C(forward), C(num_eol), C(equal), C(if_semi),
	 C(eos), C(zbranch), (cell)start);

paren: // : (   BEGIN current@ forward  [char] ) = if;  eos UNTIL ; immediate
// : paren-eos  ( -- ? )  current@ [char] ) =  eos or ;
    CODE(C(current_fetch), C(forward), C(lit), ')', C(equal), C(if_semi),
	 C(eos), C(zbranch), (cell)start);

// ---------------------------------------------------------------------------
// Command line parameters

start_command:    FUNC0(&args.command);
interactive_mode: FUNC0(&args.interactive);

// ---------------------------------------------------------------------------
// Text streams

to_forward:       OFFSET(stream_t, forward);       // >forward
to_current_fetch: OFFSET(stream_t, current_fetch); // >current@
to_eos:           OFFSET(stream_t, eos);           // >eos
per_stream: FUNC0(sizeof(stream_t)); // /stream

tick_instream: FUNC0(&sys.instream); // 'instream

to_infile:     OFFSET(textfile_t, input);   // >infile
to_infilename: OFFSET(textfile_t, name);    // >infile-name
to_current:    OFFSET(textfile_t, current); // >current
to_lineno:     OFFSET(textfile_t, lineno);  // >line#
per_textfile: FUNC0(sizeof(textfile_t)); // /textfile

lineno: FUNC0(&((textfile_t*)sys.instream)->lineno);

forward:	    // ( -- )
    w = (label_t*)((stream_t*)sys.instream)->forward; goto **w;
current_fetch:      // current@ ( -- char )
    w = (label_t*)((stream_t*)sys.instream)->current_fetch; goto **w;
eos:                // ( -- char )
    w = (label_t*)((stream_t*)sys.instream)->eos; goto **w;

file_forward:       // ( -- )
    file_forward((textfile_t*)sys.instream); goto next;
file_current_fetch: // ( -- char )
    FUNC0(((textfile_t*)sys.instream)->current);
file_eof:           // ( -- flag )
    FUNC0(BOOL(((textfile_t*)sys.instream)->current == EOF));

do_stream: // : do-stream   BEGIN interpret  eos UNTIL ;
    CODE(C(interpret), C(eos), C(zbranch), (cell)(start));

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

entry_comma: // entry, ( a c -- )  Compile an entry with the name A, code C
    {
	entry_t *e;

	ALIGN(entry_t);
	e = (entry_t*)sys.dp;
	sys.dp += sizeof(entry_t);

	e->link = sys.root.last;
	e->name = NOS;
	e->flags = 0;
	e->exec = TOS;
	e->doer = 0;

	sys.root.last = (cell)e;
	DROP(2);
	goto next;
    }

create_comma: // Create, ( 'interpreter <word> -- )
    CODE(C(parse), C(dup), C(strlen), C(oneplus), C(allot),
         C(swap), C(entry_comma));

colon_comma: // :, ( <word> -- )
    CODE(C(docol_addr), C(create_comma));

link_to:     FUNC1(&((entry_t*)TOS)->exec);	// link>  ( lfa -- xt )
flags_fetch: FUNC1(FROM_XT(TOS)->flags);	// flags@ ( xt -- n )
flags_store:					// flags! ( n xt -- )
    FROM_XT(TOS)->flags = NOS; DROP(2); goto next;

to_name: FUNC1(FROM_XT(TOS)->name);	// >name ( xt -- 'name )
to_doer: FUNC1(&FROM_XT(TOS)->doer);	// >doer ( xt -- 'doer )

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

last:     FUNC0(&sys.root.last);
dp:       FUNC0(&sys.dp);
here:     FUNC0(sys.dp);
state:    FUNC0(&sys.state);
wordq:    FUNC0(&sys.wordq);    // word? ( -- addr )

// ---------------------------------------------------------------------------
// Return stack

rdrop:
    RDROP; goto next;

rto: // >r ( n -- )
    RPUSH(TOS); DROP(1); goto next;

rfrom: // r> ( -- n )
    EXTEND(1); TOS = RPOP; goto next;

rrto: // >rr ( n -- )
    rp--; rp[0] = rp[1]; rp[1] = TOS; DROP(1); goto next;

rrfrom: // rr> ( -- n )
    EXTEND(1); TOS = rp[1]; rp[1] = rp[0]; rp++;  goto next;

rfetch: FUNC0(*rp); // r@ ( -- n)

r0:      FUNC0(&sys.r0);  //  r0 ( -- addr)
rpfetch: FUNC0(rp);       // rp@ ( -- addr )
rpstore:                  // rp! ( addr -- )
    rp = (cell*)TOS; DROP(1); goto next;

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

twodup: // 2dup ( a b -- a b a b )
    EXTEND(2); NOS = sp[3]; TOS = sp[2]; goto next;

over: // ( a b -- a b a )
    EXTEND(1); TOS = sp[2]; goto next;
under: // ( a b -- b a b )
    { cell a = NOS, b = TOS; EXTEND(1); sp[2] = TOS = b; NOS = a; goto next; }

swap: // ( a b -- b a )
    { cell tmp = TOS; TOS = NOS; NOS = tmp; goto next; }

rot: // ( a b c -- b c a )
    { cell tmp = sp[2]; sp[2] = NOS; NOS = TOS; TOS = tmp; goto next; }
minus_rot: // ( a b c -- c a b )
    { cell tmp = TOS; TOS = NOS; NOS = sp[2]; sp[2] = tmp; goto next; }

s0:      FUNC0(&sys.s0); // s0  ( -- addr)
spfetch:                 // sp@ ( -- addr )
    { cell *tmp = sp; FUNC0(tmp); }
spstore:                 // sp! ( addr -- )
    sp = (cell*)TOS; goto next;

// ---------------------------------------------------------------------------
// Arithmetics

false: FUNC0(FALSE);
true:  FUNC0(TRUE);

zero: FUNC0(0);                 //  0 ( -- n )
one:  FUNC0(1);                 //  1 ( -- n )
minus_one: FUNC0(-1);           // -1 ( -- n )
two:  FUNC0(2);                 //  2 ( -- n )

oneplus:  FUNC1(TOS + 1); // 1+  ( n -- n+1 )
oneminus: FUNC1(TOS - 1); // 1-  ( n -- n-1 )
twotimes: FUNC1(TOS * 2); // 2*  ( n -- n-1 )
twodiv:   FUNC1(TOS / 2); // 2   ( n -- n-1 )

minus:  FUNC2(NOS - TOS); // -
plus:   FUNC2(NOS + TOS); // +
times:  FUNC2(NOS * TOS); // *
divide: FUNC2(NOS / TOS); // /
mod: 	FUNC2(NOS % TOS); // mod
udivide: FUNC2((ucell)NOS / (ucell)TOS); // u/
abs:    FUNC1(cellabs(TOS));
negate: FUNC1(-TOS);
or:     FUNC2(NOS | TOS);
and:    FUNC2(NOS & TOS);
xor:    FUNC2(NOS ^ TOS);
not:	FUNC1(~TOS);

divmod: // /mod ( n1 n2 -- div mod )
    {
	celldiv_t res = celldiv(NOS, TOS);
	NOS = res.quot;
	TOS = res.rem;
	goto next;
    }

udivmod: // u/mod ( u1 u2 -- div mod )
    {
	ucell u1 = NOS, u2 = TOS;
	NOS = u1 / u2;
	TOS = u1 % u2;
	goto next;
    }

equal:      FUNC2(BOOL(NOS == TOS)); // =
unequal:    FUNC2(BOOL(NOS != TOS)); // <>
zero_equal: FUNC1(BOOL(TOS == 0));   // 0=
zero_less:  FUNC1(BOOL(TOS < 0));    // 0<
zero_greater: FUNC1(BOOL(TOS > 0));  // 0>
less:       FUNC2(BOOL(NOS < TOS));  // <
less_eq:    FUNC2(BOOL(NOS <= TOS)); // <=
greater:    FUNC2(BOOL(NOS > TOS));  // >
greater_eq: FUNC2(BOOL(NOS >= TOS)); // >=
uless:      FUNC2(BOOL((ucell)NOS <  (ucell)TOS));  // u<
uless_eq:   FUNC2(BOOL((ucell)NOS <= (ucell)TOS));  // u<=
ugreater:   FUNC2(BOOL((ucell)NOS >   (ucell)TOS)); // u>
ugreater_eq: FUNC2(BOOL((ucell)NOS >= (ucell)TOS)); // u>=

within: // ( n n0 n1 -- flag )  true when  n0 <= n < n1, with wraparound
    {
	ucell n = sp[2], n0 = NOS, n1 = TOS;
	DROP(2);
	TOS = BOOL(n - n0 < n1 - n0);
	goto next;
    }

// ---------------------------------------------------------------------------
// Memory

fetch:  FUNC1(*(cell*)TOS);	// @  ( a -- n )
cfetch: FUNC1(*(char*)TOS);	// c@ ( a -- n )

store: // ! ( n a -- )
    *(cell*)TOS = NOS; DROP(2); goto next;

plus_store: // +!  ( n a -- )
    *(cell*)TOS += NOS; DROP(2); goto next;

cstore: // c! ( n a -- )
    *(char*)TOS = NOS; DROP(2); goto next;

append: // ( a char -- a' )
    *(char*)NOS = TOS; NOS++; DROP(1); goto next;

malloc: FUNC1(malloc(TOS)); // ( n -- addr )
free:                       // ( addr -- )
    free((void*)TOS); DROP(1); goto next;

per_cell:  FUNC0(sizeof(cell));       // /cell ( -- n )
cellplus:  FUNC1(TOS + sizeof(cell)); // cell+ ( n -- n' )
cellminus: FUNC1(TOS - sizeof(cell)); // cell- ( n -- n' )

strchr: FUNC2(strchr((char*)NOS, TOS)); // ( string char -- addr )
strlen: FUNC1(strlen((char*)TOS)); // ( a -- # )

// ---------------------------------------------------------------------------
// Input/Output

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

gets: // ( a n -- a' )
    FUNC2(fgets((char*)NOS, TOS, stdin));

puts: // ( a -- )               print null-terminated string
    fputs((char*)TOS, stdout); DROP(1); goto next;

uhdot: // uh. ( n -- )		print unsigned hexadecimal
    printf("%"PRIxCELL" ", TOS); DROP(1); goto next;

bl:  FUNC0(' ');
num_eol: FUNC0('\n');		// #eol ( -- char )
num_eof: FUNC0(EOF);		// #eof ( -- char )
whitespace: FUNC0("\n\t ");

// ---------------------------------------------------------------------------
// Others

dotparen: // : .(   here " )" parse-to  here puts ;
    CODE(C(here), C(lit), (cell)")", C(parse_to), C(here), C(puts));
}

// ---------------------------------------------------------------------------
// Main program

int main(int argc, char *argv[])
{
    int opt;

    // Default: start in interactive mode
    args.command = 0;
    args.interactive = TRUE;

    while ((opt = getopt(argc, argv, "he:x:")) != -1) {
	switch (opt) {
	case 'h':
	    printf("Usage: %s [-e cmd | -x cmd | -h ]\n", argv[0]);
	    printf("Options and arguments:\n");
	    printf("-e cmd: Execute cmd and then stop\n");
	    printf("-x cmd: Execute cmd, then start command prompt.\n");
	    printf("-h    : Print this help text\n");
	    return 0;
	case 'e':
	    args.command = (cell)optarg;
	    args.interactive = FALSE;
	    break;
	case 'x':
	    args.command = (cell)optarg;
	    args.interactive = TRUE;
	    break;
	default:
	    return -1;
	}
    }

    mind();
}
