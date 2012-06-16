// mind -- a Forth interpreter
// Copyright 2011-2012 Markus Redeker <cep@ibp.de>
//
// Published under the GNU General Public License version 2 or any
// later version, at your choice. There is NO WARRANY, not at all. See
// the file "copying" for details.


/* E(label, name, flags); */

// Starting and ending
E(boot, "boot", 0)
E(abort, "abort", 0)
E(tick_abort, "'abort", 0)
E(bye, "bye", 0)

// Inner interpreter
E(next, "noop", 0)
E(semi, ";;", 0)
E(if_semi, "if;", 0)
E(zero_semi, "0;", 0)
E(execute, "execute", 0)
E(docol_addr, "^docol", 0)
E(dodefer_addr, "^dodefer", 0)
E(dovar_addr, "^dovar", 0)
E(dodoes_addr, "^dodoes", 0)

// Outer interpreter
E(paren_interpret, "(interpret)", 0)
E(interpret, "interpret", 0)
E(notfound, "notfound", 0)
E(parentick, "(')", 0)
E(parenfind, "(find)", 0)
E(lbrack, "[", IMMEDIATE)
E(rbrack, "]", 0)
E(skip_whitespace, "skip-whitespace", 0)
E(parse_to, "parse-to", 0)
E(parse, "parse", 0)
E(backslash, "\\", IMMEDIATE)
E(paren, "(", IMMEDIATE)

// Command line parameters
E(start_command, "start-command", 0)
E(interactive_mode, "interactive-mode", 0)

// Text streams
E(to_forward, ">forward", 0)
E(to_current_fetch, ">current@", 0)
E(to_eos, ">eos", 0)
E(to_num_eos, ">#eos", 0)
E(to_lineno, ">line#", 0)
E(per_textstream, "/textstream", 0)
E(tick_instream, "'instream", 0)
E(to_infile, ">infile", 0)
E(to_current, ">current", 0)
E(per_textfile, "/textfile", 0)
E(current_fetch, "current@", 0)
E(forward, "forward", 0)
E(eos, "eos", 0)
E(num_eos, "#eos", 0)
E(lineno, "line#", 0)
E(file_forward, "file-forward", 0)
E(file_current_fetch, "file-current@", 0)
E(file_eof, "file-eof", 0)
E(do_stream, "do-stream", 0)

// Dictionary
E(align, "align", 0)
E(allot, "allot", 0)
E(comma, ",", 0)
E(ccomma, "c,", 0)
E(comma_quote, ",\"", 0)
E(entry_comma, "entry,", 0)
E(link_to, "link>", 0)
E(flags_fetch, "flags@", 0)
E(flags_store, "flags!", 0)
E(to_name, ">name", 0)
E(to_doer, ">doer", 0)
E(num_immediate, "#immediate", 0)

// Inline constants
E(branch, "branch", 0)
E(zbranch, "0branch", 0)
E(lit, "lit", 0)

// System variables
E(s0, "s0",0)
E(r0, "r0",0)
E(latest, "latest",0)
E(dp, "dp",0)
E(here, "here", 0)
E(state, "state", 0)
E(wordq, "word?", 0)

// Return stack
E(rdrop, "rdrop", 0)
E(rto, ">r", 0)
E(rfrom, "r>", 0)
E(rrto, ">rr", 0)
E(rrfrom, "rr>", 0)
E(rfetch, "r@", 0)

// Stack
E(drop, "drop", 0)
E(nip, "nip", 0)
E(twodrop, "2drop", 0)
E(qdup, "?dup", 0)
E(dup, "dup", 0)
E(twodup, "2dup", 0)
E(over, "over", 0)
E(under, "under", 0)
E(swap, "swap", 0)
E(rot, "rot", 0)
E(minus_rot, "-rot", 0)
E(spfetch, "sp@", 0)
E(spstore, "sp!", 0)

// Arithmetics
E(false, "false", 0)
E(true, "true", 0)
E(zero, "0", 0)
E(one, "1", 0)
E(minus_one, "-1", 0)
E(two, "2", 0)
E(oneplus, "1+", 0)
E(oneminus, "1-", 0)
E(twotimes, "2*", 0)
E(twodiv, "2/", 0)
E(minus, "-", 0)
E(plus, "+", 0)
E(times, "*", 0)
E(divide, "/", 0)
E(mod, "mod", 0)
E(utimes, "u*", 0)
E(udivide, "u/", 0)
E(abs, "abs", 0)
E(or, "or", 0)
E(and, "and", 0)
E(xor, "xor", 0)
E(invert, "invert", 0)
E(divmod, "/mod", 0)
E(udivmod, "u/mod", 0)
E(equal, "=", 0)
E(unequal, "<>", 0)
E(zero_equal, "0=", 0)
E(zero_less, "0<", 0)
E(zero_greater, "0>", 0)
E(less, "<", 0)
E(less_eq, "<=", 0)
E(greater, ">", 0)
E(greater_eq, ">=", 0)
E(uless, "u<", 0)
E(uless_eq, "u<=", 0)
E(ugreater, "u>", 0)
E(ugreater_eq, "u>=", 0)
E(within, "within", 0)

// Memory
E(fetch, "@", 0)
E(cfetch, "c@", 0)
E(store, "!", 0)
E(plus_store, "+!", 0)
E(cstore, "c!", 0)
E(append, "append", 0)
E(malloc, "malloc", 0)
E(free, "free", 0)
E(cells, "cells", 0)
E(cellplus, "cell+", 0)
E(cellminus, "cell-", 0)

E(strlen, "strlen", 0)
E(strchr, "strchr", 0)

// Input/Output
E(emit, "emit", 0)
E(type, "type", 0)
E(gets, "gets", 0)
E(puts, "puts", 0)
E(cr, "cr", 0)
E(uhdot, "uh.", 0)
E(bl, "bl", 0)
E(num_eol, "#eol", 0)
E(num_eof, "#eof", 0)
E(whitespace, "whitespace", 0)

// Others
E(dotparen, ".(", 0)

// Local Variables:
// c-syntactic-indentation: nil
// End:
