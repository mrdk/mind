#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "types.h"

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
#define E(label, name, flags)  i_##label,
#include "heads.c"
#undef E
    num_words };

/* Interpreter flags */
#define IMMEDIATE 1

typedef void *label_t;		/* Target of computed goto. */

typedef struct {
    cell link;		 /* (entry*) points to previous entry.     */
    cell name;		 /* (char*)  Pointer to start of word name */
    char flags;
    cell cfa;		 /* (label_t) code field address              */
    cell doer;		 /* (cell*)   Forth routine for `does>` part. */
    cell body[];
} entry_t;

#define NEW_ENTRY(label, wname, wflags)				\
{   .link   = i_##label ? (cell)&dict[i_##label - 1] : 0,	\
    .flags = wflags,						\
    .name  = (cell)wname,					\
    .cfa   = (cell)&&label,					\
    .doer  = 0 },

#define FROM_CFA(addr)							\
    ((entry_t*)((char*)(addr) - offsetof(entry_t, cfa)))

/* Find string *name* in dictionary, starting at *e*. */
static entry_t *find_word(entry_t *e, char *name)
{
    for (; e->link; e = (entry_t*)e->link) {
	if (!strcmp((char*)e->name, name))
	    return e;
    }
    return NULL;
}

/* Find CFA for *name* in dictionary, starting at *e*. */
static cell* find_cfa(entry_t *e, char *name)
{
    e = find_word(e, name);
    return e? &e->cfa : NULL;
}

/* ----------------------------------------------------------------------- */
/* Define hand-compiled Forth code */
#define CODE(...)					\
    {							\
	static cell start[] = {__VA_ARGS__, C(semi) };	\
	RPUSH(ip), ip = start; goto next;		\
    }

#define C(label)  ((cell)(&dict[i_##label].cfa)) // Call to Forth word

// ---------------------------------------------------------------------------
// Reading text files and interactive input

typedef struct {
    cell get_char;		// Forth word ( stream -- char )
    cell eos;			// Forth word ( stream -- flag )
    cell num_eos;		// integer: "end of stream constant"
    cell lineno;		// integer: line number
} textstream_t;

typedef struct {
    textstream_t stream;
    cell input;			// (FILE*) Input file
} textfile_t;

static void open_textfile(textfile_t *inf, char* name, entry_t dict[])
{
    inf->stream.get_char = C(file_get_char);
    inf->stream.eos = C(file_eof);
    inf->stream.num_eos = EOF;
    inf->stream.lineno = 1;
    inf->input = (cell)fopen(name, "r");
}

static int file_get_char(textfile_t *inf)
{
    int cin = fgetc((FILE*)inf->input);

    if (cin == '\n')
	inf->stream.lineno++;

    return cin;
}

// ---------------------------------------------------------------------------
// Memory

// System variables
struct {
    cell r0;		     // (cell*) Start of the return stack
    cell dp;		     // (cell*) Dictionary pointer
    cell s0;		     // (cell*) Start of the parameter stack
    cell latest;	     // (entry_t*) The latest definition
    cell state;		     // Compiler state
    cell wordq;		     // Called if word not found (Name: Retro)
    cell tick_abort;	     // Called by `abort` to get an interactive prompt
    textfile_t inf;	     // Input file
    cell instream;	     // (textstream_t*) Current input stream
    cell mem[MEMCELLS];	     // The memory
} sys;

static void init_sys(entry_t dict[])
{
    sys.r0 = (cell)(sys.mem + 100);
    sys.dp = sys.r0 + sizeof(cell);
    sys.s0 = (cell)(sys.mem + MEMCELLS - 0x10); // Top of memory + safety space
    sys.latest = (cell)&dict[num_words - 1];
    sys.state = 0;
    sys.wordq = C(notfound);
    sys.tick_abort = C(bye);
    open_textfile(&sys.inf, "start.mind", dict);
    sys.instream = (cell)&sys.inf.stream;
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

int main(int argc, char *argv[])
{
    cell *ip;			/* Instruction Pointer */
    label_t *w;			/* Word Pointer */
    cell *rp;			/* Return Stack Pointer */
    cell *sp;			/* Stack Pointer */

    static entry_t dict[] = { /* Dictionary */
#define E NEW_ENTRY
#include "heads.c"
#undef E
    };

// ---------------------------------------------------------------------------
// Starting and ending
boot:
    init_sys(dict);
    sp = (cell*)sys.s0;
    rp = (cell*)sys.r0;
    {
	static cell interpreter[] = {
	    C(do_stream), C(tick_abort), C(fetch), C(execute) };
	ip = interpreter;
	goto next;
    }

abort:
    w = (label_t*)sys.tick_abort; goto **w;

tick_abort: FUNC0(&sys.tick_abort);

bye:
    return 0;

// ---------------------------------------------------------------------------
// Inner interpreter

next:				/* Address Interpreter */
    w = (label_t*)*ip++; goto **w;

docol:				/* Runtime of ":" */
    RPUSH(ip); ip = FROM_CFA(w)->body; goto next;

dodefer:			/* Runtime of Defer */
    RPUSH(ip); w = (label_t*)FROM_CFA(w)->doer; goto **w;

dovar:				/* Runtime of Variable */
    PUSH(FROM_CFA(w)->body); goto next;

dodoes: //			Runtime for Create ... does>
    PUSH(FROM_CFA(w)->body);
    RPUSH(ip); ip = (cell*)FROM_CFA(w)->doer; goto next;

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

paren_interpret: // (interpret)  ( ... addr -- ... )
    {
	entry_t *e = find_word((entry_t*)sys.latest, (char*)TOS);

	DROP(1);
	if (e) {
	    if (sys.state && !(e->flags & IMMEDIATE)) {
		COMMA(&e->cfa, cell);
		goto next;
	    }
	    else
		w = (label_t*)&e->cfa;
	} else
	    w = (label_t*)sys.wordq;

	goto **w;
    }

interpret:  // : interpret   parse (interpret) ;
    CODE(C(parse), C(paren_interpret));

notfound: // Tell that the word at sys.dp could not be interpreted
    {
	printf("l%"PRIdCELL": not found: %s\n",
	       ((textstream_t*)sys.instream)->lineno, (char*)sys.dp);
	fclose((FILE*)sys.inf.input);
	goto abort;
    }

parentick: // (') ( "word" -- cfa | 0 )
    CODE(C(parse), C(parenfind));

parenfind: // (find) ( addr -- cfa | 0 )
    FUNC1(find_cfa((entry_t*)sys.latest, (char*)TOS));

lbrack:	// [
    sys.state = 0; goto next;
rbrack:	// ]
    sys.state = 1; goto next;

parse_to: // : parse-to ( addr string -- )
	  //   >r BEGIN get-char r@ append-notfrom  0=  eos or UNTIL  rdrop
	  //   0 swap c! ;
    CODE(C(rto),
	 C(get_char), C(rfetch), C(append_notfrom), C(zero_equal),
	 C(eos), C(or), C(zbranch), (cell)(start + 1),
	 C(rdrop), C(zero), C(swap), C(cstore));

parse: // : parse ( -- addr )
       //   here  BEGIN get-char whitespace append-notfrom UNTIL
       //   whitespace parse-to  here ;
    CODE(C(here),
	 C(get_char), C(whitespace), C(append_notfrom),
	 C(zbranch), (cell)(start + 1),
	 C(whitespace), C(parse_to), C(here));

backslash: // : \    BEGIN get-char
	   //              dup #eos =  swap #eol =  or UNTIL ; immediate
    CODE(C(get_char), C(dup), C(num_eos), C(equal),
	 C(swap), C(num_eol), C(equal), C(or), C(zbranch), (cell)start);

paren:     // : (    BEGIN get-char
	   //        dup #eos =  swap [char] ) =  or UNTIL ; immediate
    CODE(C(get_char), C(dup), C(num_eos), C(equal),
	 C(swap), C(lit), ')', C(equal), C(or), C(zbranch), (cell)start);

// ---------------------------------------------------------------------------
// Text streams

to_get_char: OFFSET(textstream_t, get_char); // >get-char
to_eos:      OFFSET(textstream_t, eos);	     // >eos
to_num_eos:  OFFSET(textstream_t, num_eos);  // >#eos
to_lineno:   OFFSET(textstream_t, lineno);   // >line#
per_textstream: FUNC0(sizeof(textstream_t)); // /textstream

tick_instream: FUNC0(&sys.instream);

to_infile: OFFSET(textfile_t, input);	 // >infile
per_textfile: FUNC0(sizeof(textfile_t)); // /textfile

lineno: FUNC0(&((textstream_t*)sys.instream)->lineno);

get_char: // ( -- char )
    EXTEND(1); TOS = sys.instream;
    w = (label_t*)((textstream_t*)sys.instream)->get_char; goto **w;

eos: // ( -- char )
    EXTEND(1); TOS = sys.instream;
    w = (label_t*)((textstream_t*)sys.instream)->eos; goto **w;

num_eos: FUNC0(((textstream_t*)sys.instream)->num_eos);

file_get_char: // ( stream -- char )
    FUNC1(file_get_char((textfile_t*)TOS));
file_eof:      // ( stream -- flag )
    FUNC1(BOOL(feof((FILE*)((textfile_t*)TOS)->input)));

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

comma_quote: // ,"
    CODE(C(here), C(lit), (cell)"\"", C(parse_to),
    	 C(here), C(strlen), C(oneplus), C(allot));

entry_comma: // ( a c -- )	Compile an entry with the name A, code field C
    {
	entry_t *e;

	ALIGN(entry_t);
	e = (entry_t*)sys.dp;
	sys.dp += sizeof(entry_t);

	e->link = sys.latest;
	e->name = NOS;
	e->flags = 0;
	e->cfa = TOS;
	e->doer = 0;

	sys.latest = (cell)e;
	DROP(2);
	goto next;
    }

link_to:     FUNC1(&((entry_t*)TOS)->cfa);	// link>  ( lfa -- cfa )
flags_fetch: FUNC1(FROM_CFA(TOS)->flags);	// flags@ ( cfa -- n )
flags_store:					// flags! ( n cfa -- )
    FROM_CFA(TOS)->flags = NOS; DROP(2); goto next;

to_name: FUNC1(FROM_CFA(TOS)->name);	// >name ( cfa -- 'name )
to_doer: FUNC1(&FROM_CFA(TOS)->doer);	// >doer ( cfa -- 'doer )

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

// ---------------------------------------------------------------------------
// Return stack

rdrop:
    RDROP; goto next;

rto: // >r ( n -- )
    RPUSH(TOS); DROP(1); goto next;

rfrom: // r> ( -- n )
    EXTEND(1); TOS = RPOP; goto next;

rfetch: FUNC0(*rp);

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


spfetch: FUNC0(&NOS); // sp@ ( -- addr )

spstore: // sp! ( addr -- )
    sp = (cell*)TOS; DROP(1); goto next;

// ---------------------------------------------------------------------------
// Arithmetics

false: FUNC0(FALSE);
true:  FUNC0(TRUE);

zero: FUNC0(0);
one:  FUNC0(1);
minus_one: FUNC0(-1);
two:  FUNC0(2);

oneplus:  FUNC1(TOS + 1); // 1+  ( n -- n+1 )
oneminus: FUNC1(TOS - 1); // 1-  ( n -- n-1 )
twotimes: FUNC1(TOS * 2); // 2*  ( n -- n-1 )
twodiv:   FUNC1(TOS / 2); // 2   ( n -- n-1 )

minus:  FUNC2(NOS - TOS); // -
plus:   FUNC2(NOS + TOS); // +
times:  FUNC2(NOS * TOS); // *
divide: FUNC2(NOS / TOS); // /
mod: 	FUNC2(NOS % TOS); // mod
utimes: FUNC2((ucell)NOS * (ucell)TOS);  // u*
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

udivmod: // /mod ( u1 u2 -- div mod )
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

append_from: // ( a inchar str -- a' flag )
    {
	cell appending = BOOL(NOS != ((textstream_t*)sys.instream)->num_eos &&
			      strchr((char*)TOS, NOS));
	if (appending) {
	    *(char*)sp[2] = NOS; sp[2]++;
	}
	DROP(1); TOS = appending; goto next;
    }

append_notfrom: // ( a inchar str -- a' flag )
    {
	cell appending = BOOL(NOS != ((textstream_t*)sys.instream)->num_eos &&
			      !strchr((char*)TOS, NOS));
	if (appending) {
	    *(char*)sp[2] = NOS; sp[2]++;
	}
	DROP(1); TOS = appending; goto next;
    }

malloc: FUNC1(malloc(TOS)); // ( n -- addr )
free:                       // ( addr -- )
    free((void*)TOS); DROP(1); goto next;

cells:     FUNC1(TOS * sizeof(cell));
cellplus:  FUNC1(TOS + sizeof(cell)); // cell+
cellminus: FUNC1(TOS - sizeof(cell)); // cell-

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

hdot: // h. ( n -- )		print hexadecimal
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
