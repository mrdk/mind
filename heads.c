/* -*- mode:c; c-syntactic-indentation: nil -*- */

/* E(label, name, flags); */

// Starting and ending
E(boot, "boot", 0)
E(abort, "abort", 0)
E(quit, "quit", 0)
E(bye, "bye", 0)

// Inner interpreter
E(next, "noop", 0)
E(semi, ";;", 0)
E(zero_semi, "0;", 0)
E(execute, "execute", 0)
E(docol_addr, "^docol", 0)
E(dodefer_addr, "^dodefer", 0)
E(dovar_addr, "^dovar", 0)
E(dodoes_addr, "^dodoes", 0)

// Outer interpreter
E(interpret, "interpret", 0)
E(notfound, "notfound", 0)
E(parentick, "(')", 0)
E(parenfind, "(find)", 0)
E(lbrack, "[", IMMEDIATE)
E(rbrack, "]", 0)
E(backslash, "\\", IMMEDIATE)
E(paren, "(", IMMEDIATE)

// Dictionary
E(align, "align", 0)
E(allot, "allot", 0)
E(comma, ",", 0)
E(comma_quote, ",\"", 0)
E(parse, "parse", 0)
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
E(lineno, "line#", 0)
E(pageno, "page#", 0)

// Return stack
E(rdrop, "rdrop", 0)
E(rto, ">r", 0)
E(rfrom, "r>", 0)
E(r, "r", 0)

// Stack
E(drop, "drop", 0)
E(nip, "nip", 0)
E(twodrop, "2drop", 0)
E(qdup, "?dup", 0)
E(dup, "dup", 0)
E(over, "over", 0)
E(under, "under", 0)
E(swap, "swap", 0)
E(rot, "rot", 0)
E(minus_rot, "-rot", 0)
E(spfetch, "sp@", 0)

// Arithmetics
E(zero, "0", 0)
E(one, "1", 0)
E(minus_one, "-1", 0)
E(two, "2", 0)
E(oneplus, "1+", 0)
E(oneminus, "1-", 0)
E(minus, "-", 0)
E(plus, "+", 0)
E(times, "*", 0)
E(divide, "/", 0)
E(utimes, "u*", 0)
E(or, "or", 0)
E(equal, "=", 0)
E(zero_equal, "0=", 0)
E(zero_less, "0<", 0)
E(uless, "u<", 0)
E(ugreater, "u>", 0)

// Memory
E(fetch, "@", 0)
E(cfetch, "c@", 0)
E(store, "!", 0)
E(cells, "cells", 0)
E(cellplus, "cell+", 0)
E(cellminus, "cell-", 0)

// Input/Output
E(strlen, "strlen", 0)
E(emit, "emit", 0)
E(type, "type", 0)
E(puts, "puts", 0)
E(cr, "cr", 0)
E(hdot, "h.", 0)
E(blank, "blank", 0)

// Others
E(dotparen, ".(", 0)
