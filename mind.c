// mind -- a Forth interpreter
// Copyright 2011-2014 Markus Redeker <cep@ibp.de>
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
#include <errno.h>

#include "args.h"
#include "io.h"

#define MEMCELLS 0x10000	// Number of cells in the main memory
#define RCELLS   0x100          // Number of cells in the return stack
#define OREFS    0x100          // Number of references in the object stack

// ---------------------------------------------------------------------------
// Objects

// References to objects
typedef struct {
    cell this;
    cell class;
} ref_t;

/* ---------------------------------------------------------------------- */
/* Dictionary structure */

// Enumerate the dictionary entries. The word `foo` gets the number
// `i_foo`, and its dictionary entry is found at dict[i_foo].
enum wordnum_t {
#define E(label, ...)  i_##label,
#define D E
#include "headers.c"
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
    cell xt;        // (label_t) C code that executes this word.
    cell doer;      // (cell*)   Forth routine for `does>` part.
    cell body[];
} entry_t;

#define NEW_WORD(label, wname, wflags)				\
{   .link  = i_##label ? (cell)&dict[i_##label - 1] : 0,	\
    .flags = wflags,						\
    .name  = (cell)wname,					\
    .xt    = (cell)&&label,					\
    .doer  = 0 },

#define NEW_DEFER(label, wname, wdoer, wflags)                  \
{   .link  = i_##label ? (cell)&dict[i_##label - 1] : 0,	\
    .flags = wflags,						\
    .name  = (cell)wname,					\
    .xt    = (cell)&&dodefer,					\
    .doer  = C(wdoer) },

// Compute the address of an entry_t field when given an execution
// token instead of the beginning of the struct.
#define FROM_XT(addr)                                     \
    ((entry_t*)((char*)(addr) - offsetof(entry_t, xt)))

/* Find XT for *name* in dictionary, starting at *e*. */
static cell* find_xt(entry_t *e, char *name)
{
    for (; e; e = (entry_t*)e->link) {
	if (!strcmp((char*)e->name, name))
	    return &e->xt;
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

#define C(label)  ((cell)(&dict[i_##label].xt)) // Call to Forth word

// ---------------------------------------------------------------------------
// System variables

struct {
    // System variables
    cell r0;		     // (cell*) Start of the return stack
    cell op0;                // (ref_t*) Start of the object stack
    cell op;                 // (ref_t*) Top of the object stack
    cell dp;		     // (cell*) Dictionary pointer
    cell s0;		     // (cell*) Start of the parameter stack
    cell state;		     // Compiler state
    cell wordq;		     // Called if word not found
    context_t root;          // root context
    textfile_t textfile0;    // Prototype for text streams
    textfile_t inf;	     // Input file
    ref_t this_file;         // Current input file

    // Memory layout
    ref_t ostack[OREFS];     // Object stack
    cell rstack[RCELLS];     // Return stack
    cell mem[MEMCELLS];	     // Main memory
} sys;

static void file_init(textfile_t *inf, entry_t dict[])
{
    inf->stream.get = C(file_get);
    inf->stream.i = C(file_i);
    inf->stream.iq = C(file_iq);
    inf->input = 0;
    inf->name = 0;
    inf->current = EOF;
    inf->lineno = 0;
}

static void init_sys(entry_t dict[])
{
    sys.r0 = (cell)(sys.rstack + RCELLS);
    sys.op0 = (cell)(sys.ostack + OREFS - 0x10);
    sys.op = sys.op0;
    sys.dp = (cell)sys.mem;
    sys.s0 = (cell)(sys.mem + MEMCELLS - 0x10); // Top of memory + safety space
    sys.state = 0;
    sys.wordq = C(notfound);
    sys.root.link = 0;
    sys.root.last = (cell)&dict[num_words - 1];
    sys.root.find_word = C(find_word);
    file_init(&sys.textfile0, dict);
    memcpy(&sys.inf, &sys.textfile0, sizeof(textfile_t));
    sys.this_file = (ref_t) { .class = (cell)&sys.inf.stream };
}

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

// Macros for "procedures": words that consume all of their parameters
// The parameter X should be a single statement; otherwise the code
// would become too complicated.
#define PROC1(x)  x; DROP(1); goto next   // ( n -- )
#define PROC2(x)  x; DROP(2); goto next   // ( n1 n2 -- )

// Macros for "functions": words with 1 cell as a result
#define FUNC0(x)  EXTEND(1); TOS = (cell)(x); goto next // ( -- n )
#define FUNC1(x)  TOS = (cell)(x); goto next            // ( n1 -- n2 )
#define FUNC2(x)  NOS = (cell)(x); DROP(1); goto next   // ( n1 n2 -- n3 )

// Some functions must return a Forth-style boolean.
#define BOOL(n)	((n) ? TRUE : FALSE)

// Macro for a word that computes the address of a field from the
// address of a struct
#define OFFSET(type, field)   FUNC0(obj.class + offsetof(type, field))

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
    ref_t obj;                  // Active object

    static entry_t dict[] = { /* Dictionary */
#define E NEW_WORD
#define D NEW_DEFER
#include "headers.c"
#undef D
#undef E
    };

// ---------------------------------------------------------------------------
// Starting and ending
    init_sys(dict);

    file_open(&sys.inf,
              mind_relative((char*)args.raw_argv[0], "init.mind"));
    if (sys.inf.current == EOF) {
	fprintf(stderr, "Error: File '%s' not found\n", (char*)sys.inf.name);
	exit(-1);
    }

    sp = (cell*)sys.s0;
    rp = (cell*)sys.r0;
    obj.this = 0;
    obj.class = (cell)&sys.inf.stream;

    {
	static cell interpreter[] = { C(do_stream), C(boot) };
	ip = interpreter;
	goto next;
    }

bye:
    return;

// ---------------------------------------------------------------------------
// Objects

this:        FUNC0(obj.this);        // ( -- addr )
this_plus:   FUNC1(obj.this + TOS);  // this+  ( u -- addr )
store_this:  PROC1(obj.this = TOS);  // !this  ( addr -- )
class:       FUNC0(obj.class);       // ( -- addr )
class_plus:  FUNC1(obj.class + TOS); // class+ ( u -- addr )
store_class: PROC1(obj.class = TOS); // !class ( addr -- )

per_ref:   FUNC0(sizeof(ref_t));      // /ref ( -- n )
ref_store: PROC1(*(ref_t*)TOS = obj); // ref! ( addr -- )
ref_fetch: PROC1(obj = *(ref_t*)TOS); // ref@ ( addr -- )

op0: FUNC0(&sys.op0);  // ( -- addr )
op:  FUNC0(&sys.op);   // ( -- addr )

scope:      // {
    sys.op -= sizeof(ref_t); *((ref_t*)sys.op) = obj; goto next;
end_scope:  // }
    obj = *((ref_t*)sys.op); sys.op += sizeof(ref_t); goto next;

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

state:  FUNC0(&sys.state);        // ( -- addr )
lbrack: sys.state = 0; goto next; // [
rbrack: sys.state = 1; goto next; // ]

wordq: FUNC0(&sys.wordq);  // word? ( -- addr )

exec_compile: // exec/compile ( xt -- )
    {
        cell xt = TOS;
	DROP(1);

	if (xt) {
            entry_t *e = FROM_XT(xt);

	    if (sys.state && !(e->flags & IMMEDIATE)) {
		COMMA(&e->xt, cell);
		goto next;
	    }
	    else
		w = (label_t*)&e->xt;
	} else
	    w = (label_t*)sys.wordq;

	goto **w;
    }

interpret:  // : interpret   parse find exec/compile ;
    CODE(C(parse), C(find), C(exec_compile));

notfound: // Tell that the word at sys.dp could not be interpreted
    {
	printf("l%"PRIdCELL": not found: %s\n",
	       ((textfile_t*)obj.class)->lineno, (char*)sys.dp);
        file_close(&sys.inf);
        w = (label_t)C(abort); goto **w;
    }

find: // find ( str -- xt | 0 )
    CODE(C(lit), (cell)&sys.root, C(find_word));

find_word: // find-word ( str ctx -- xt | 0 )
    FUNC2(find_xt((entry_t*)((context_t*)TOS)->last, (char*)NOS));

parse_to: // : parse-to ( addr str -- )
          //   { file: >r
          //   BEGIN i append  get
          //      r@ i strchr  i? 0= or UNTIL rdrop
          //   0 over c!  i? 0= if; get } ;
    CODE(C(scope), C(file_colon), C(rto),
	 C(i), C(append), C(get),
	 C(rfetch), C(i), C(strchr),
         C(iq), C(zero_equal), C(or),
	 C(zbranch), (cell)(start + 3), C(rdrop),
	 C(zero), C(swap), C(cstore),
         C(iq), C(zero_equal), C(if_semi), C(get), C(end_scope));

skip_whitespace: // : skip-whitespace ( -- )
	         //   BEGIN  whitespace i strchr 0= if;  get AGAIN ;
    CODE(C(whitespace), C(i), C(strchr), C(zero_equal),
	 C(if_semi), C(get), C(branch), (cell)(start));

parse: // : parse ( -- addr )
       //   { file: skip-whitespace  here whitespace parse-to }  here ;
    CODE(C(scope), C(file_colon), C(skip_whitespace),
         C(here), C(whitespace), C(parse_to), C(here), C(end_scope));

backslash: // : \   { file: BEGIN i get  #eol = if;
           //                      i? 0= UNTIL } ;  immediate
    CODE(C(scope), C(file_colon),
         C(i), C(get), C(num_eol), C(equal), C(if_semi),
	 C(iq), C(zero_equal), C(zbranch), (cell)(start + 2),
         C(end_scope));

paren: // : (   { file: BEGIN i get  [char] ) = if;
       //                      i? 0= UNTIL } ;  immediate
    CODE(C(scope), C(file_colon),
         C(i), C(get), C(lit), ')', C(equal), C(if_semi),
	 C(iq), C(zero_equal), C(zbranch), (cell)(start + 2),
         C(end_scope));

// ---------------------------------------------------------------------------
// Command line parameters

raw_argc: FUNC0(args.raw_argc);
raw_argv: FUNC0(args.raw_argv);
argc:     FUNC0(args.argc);
argv:     FUNC0(args.argv);

arg_cmdline:     FUNC0(&args.command);
arg_interactive: FUNC0(&args.interactive);

// ---------------------------------------------------------------------------
// Files

stdin_:  FUNC0(stdin);
stdout_: FUNC0(stdout);
stderr_: FUNC0(stderr);

// ---------------------------------------------------------------------------
// Text streams

init_mind: FUNC0(&sys.inf);     // init.mind ( -- addr )

tick_get:     OFFSET(stream_t, get);    // 'get
tick_i:       OFFSET(stream_t, i);      // 'i
tick_iq:      OFFSET(stream_t, iq);     // 'i?
per_stream: FUNC0(sizeof(stream_t));  // /stream

file_ref:   FUNC0(&sys.this_file);          // file-ref  ( -- addr )
file_colon: obj = sys.this_file; goto next; // file:

tick_infile:     OFFSET(textfile_t, input);   // 'infile
tick_infilename: OFFSET(textfile_t, name);    // 'infile-name
tick_current:    OFFSET(textfile_t, current); // 'current
tick_lineno:     OFFSET(textfile_t, lineno);  // 'line#
per_textfile: FUNC0(sizeof(textfile_t)); // /textfile

lineno: FUNC0(&((textfile_t*)obj.class)->lineno);

get:                // ( -- )
    w = (label_t*)((stream_t*)obj.class)->get; goto **w;
i:                  // i ( -- char )
    w = (label_t*)((stream_t*)obj.class)->i; goto **w;
iq:                 // i? ( -- flag )
    w = (label_t*)((stream_t*)obj.class)->iq; goto **w;

textfile0: obj.this = 0; obj.class = (cell)&sys.textfile0; goto next;
file_open:          // file-open     ( str {tstream} -- )
    PROC1(file_open((textfile_t*)obj.class, (char*)TOS));
file_close:         // file-close    ( {tstream} --)
    file_close((textfile_t*)obj.class); goto next;
file_get:           // file-get  ( -- )
    file_get((textfile_t*)obj.class); goto next;
file_i:             // file-i ( -- char )
    FUNC0(((textfile_t*)obj.class)->current);
file_iq:            // file-i?   ( -- flag )
    FUNC0(BOOL(((textfile_t*)obj.class)->current != EOF));

per_lines: FUNC0(sizeof(lines_t)); // /lines
lines_open:          // lines-open     ( str file -- )
    PROC2(lines_open((lines_t*)TOS, (char*)NOS));
lines_close:         // lines-close    ( file --)
    PROC1(lines_close((lines_t*)TOS));
lines_get:           // lines-get  ( -- )
    lines_get((lines_t*)obj.class); goto next;
lines_i:             // lines-i ( -- char )
    FUNC0(((lines_t*)obj.class)->line);
lines_iq:            // lines-i?   ( -- flag )
    FUNC0(BOOL(((lines_t*)obj.class)->line != 0));

do_stream: // : do-stream   BEGIN interpret { file: i? } 0= UNTIL ;
    CODE(C(interpret),
         C(scope), C(file_colon), C(iq), C(end_scope),
         C(zero_equal), C(zbranch), (cell)start);

errno_: FUNC0(&errno); // ( -- addr )


// ---------------------------------------------------------------------------
// Dictionary

last:   FUNC0(&sys.root.last);    // ( -- addr )
dp:     FUNC0(&sys.dp);           // ( -- addr )
here:   FUNC0(sys.dp);            // ( -- addr )

align:  ALIGN(cell); goto next;
allot:  PROC1(sys.dp += TOS);     // ( n -- )

comma:  PROC1(COMMA(TOS, cell));  // , ( n -- )
ccomma: PROC1(COMMA(TOS, char));  // c, ( n -- )

entry_comma:                      // entry, ( str xt -- )
    {
	ALIGN(entry_t);

        *(entry_t*)sys.dp = (entry_t) {
            .link = sys.root.last,
            .name = NOS,
            .xt = TOS,
        };

	sys.root.last = sys.dp;
	sys.dp += sizeof(entry_t);

	DROP(2);
	goto next;
    }

create_comma: // : Create, ( 'interpreter <word> -- )
              //   parse  dup strlen 1+ allot  swap entry, ;
    CODE(C(parse), C(dup), C(strlen), C(oneplus), C(allot),
         C(swap), C(entry_comma));

colon_comma: // : :, ( <word> -- )   ^docol Create, ;
    CODE(C(docol_addr), C(create_comma));

#define FROM_BODY(addr)                                 \
    ((entry_t*)((char*)(addr) - offsetof(entry_t, body)))

link_to:     FUNC1(&((entry_t*)TOS)->xt);	// link>  ( lfa -- xt )
body_to:     FUNC1(&FROM_BODY(TOS)->xt);        // body>  ( body -- xt )
flags_fetch: FUNC1(FROM_XT(TOS)->flags);	// flags@ ( xt -- n )
flags_store: PROC2(FROM_XT(TOS)->flags = NOS);  // flags! ( n xt -- )

to_link: FUNC1(&FROM_XT(TOS)->link);	// >link ( xt -- 'link )
to_name: FUNC1(&FROM_XT(TOS)->name);	// >name ( xt -- 'name )
to_doer: FUNC1(&FROM_XT(TOS)->doer);	// >doer ( xt -- 'doer )
to_body: FUNC1(&FROM_XT(TOS)->body);	// >body ( xt -- 'body )

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

lit: FUNC0(*ip++);              // ( -- n )

// ---------------------------------------------------------------------------
// Return stack

rdrop: RDROP; goto next;
rto:   PROC1(RPUSH(TOS)); // >r ( n -- )
rfrom: FUNC0(RPOP);       // r> ( -- n )

rrto:   // >rr ( n -- )
    rp--; rp[0] = rp[1]; rp[1] = TOS; DROP(1); goto next;

rrfrom: // rr> ( -- n )
    EXTEND(1); TOS = rp[1]; rp[1] = rp[0]; rp++;  goto next;

rfetch:  FUNC0(*rp);             // r@  ( -- n)
r0:      FUNC0(&sys.r0);         // r0  ( -- addr)
rpfetch: FUNC0(rp);              // rp@ ( -- addr )
rpstore: PROC1(rp = (cell*)TOS); // rp! ( addr -- )

// ---------------------------------------------------------------------------
// Stack

drop:    DROP(1); goto next; // ( n -- )
twodrop: DROP(2); goto next; // 2drop ( n1 n2 -- )
nip:     FUNC2(TOS);         // ( n1 n2 -- n2 )

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
zero_unequal: FUNC1(BOOL(TOS != 0)); // 0<>
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

store:      PROC2(*(cell*)TOS = NOS);  // !  ( n a -- )
plus_store: PROC2(*(cell*)TOS += NOS); // +! ( n a -- )
cstore:     PROC2(*(char*)TOS = NOS);  // c! ( n a -- )

append: // ( a char -- a' )
    *(char*)NOS = TOS; NOS++; DROP(1); goto next;

cmove: // ( from to u -- )
    memcpy((char*)NOS, (char*)sp[2], TOS); DROP(3); goto next;

fill: // ( addr u char -- )
    memset((char*)sp[2], TOS, NOS); DROP(3); goto next;

malloc: FUNC1(malloc(TOS));      // ( n -- addr )
free:   PROC1(free((void*)TOS)); // ( addr -- )

per_cell:  FUNC0(sizeof(cell));       // /cell ( -- n )
cellplus:  FUNC1(TOS + sizeof(cell)); // cell+ ( n -- n' )
cellminus: FUNC1(TOS - sizeof(cell)); // cell- ( n -- n' )

strchr: FUNC2(strchr((char*)NOS, TOS)); // ( str char -- addr )
strlen: FUNC1(strlen((char*)TOS));      // ( str -- # )

// ---------------------------------------------------------------------------
// Input/Output

cr: putchar('\n'); goto next;

emit: PROC1(putchar(TOS)); // ( c -- )

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
    PROC1(fputs((char*)TOS, stdout));

uhdot: // uh. ( n -- )		print unsigned hexadecimal
    PROC1(printf("%"PRIxCELL" ", TOS));

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
    init_args(argc, argv);
    mind();
}
