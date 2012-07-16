Design decisions
================

C Code
------

+ Use C99 to full extent.

+ Use gcc specific extensions only when necessary.
  Currently used:

    - computed goto
    - __alignof

+ A cell may contain both an int and a pointer
  
  The basic Forth data type, the cell, becomes the smallest integer
  type with at least as many bytes to store both an int or a void*.

  This will make it possible to interface to C functions in a
  machine-independent and efficient way: It will always be possible to
  store an int, a char or any pointer e.g. on the stack.

+ The core of the program should rely only on ANSI C

  Exception:

  - getopt(), which is POSIX. (It may later be replaced by Forth
    code.)

+ We assume that signed integers are always in two's complement
    
  This choice has influence on the Forth code: it means that we need
  only one kind of addition and subtraction; only multiplication and
  comparisons come in two version.

  The C standard allows also sign+magnitude and one's complement, but
  they are rarely used, if at all. And Java explicitly requires two's
  complement, so this may be the future quasi-standard. (This includes
  some guessing, I didn't find yet explicit information in the
  internet. - mr)

Forth vocabulary
----------------

+ Normal Forth words are in lower case.
    
  This makes it much easier to switch between Forth and other
  applications.

+ "Create"-like words begin with a capital letter.

+ Control structure words like IF and WHILE are capitalised
  
  This is a kind of convention in Forth code and it lets them stand
  out.

+ There are no words that handle double integers.

  The C integers are large enough for us. [From GraspForth]

+ Strings are null-terminated, as in C

  This is better for cooperation with C libraries.

Copyright 2012 Markus Redeker. This work is licensed under the Creative
Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of
this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or
send a letter to Creative Commons, 444 Castro Street, Suite 900,
Mountain View, California, 94041, USA.
