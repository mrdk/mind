Design decisions
================

C Code
------

+ Use of C99 to full extent.

+ Use of gcc specific extensions only when necessary. Currently used
  are:

 - computed goto,
 - the keyword :c:func:`__alignof__`.

+ A cell may contain both an :c:type:`int` and a pointer.
  
  The basic Forth data type, the cell, becomes the smallest integer
  type with at least as many bytes to store both an :c:type:`int` or a
  :c:type:`void*`.

  This will make it possible to interface to C functions in a machine
  independent and efficient way: It will always be possible to store
  an :c:type:`int`, a :c:type:`char` or any pointer on the stack.

+ The core of the program should rely only on ANSI C. Exception:

 - :c:func:`getopt()`, which is POSIX. (It may later be replaced by
   Forth code.)

+ We assume that signed integers are always in two's complement.
    
  This choice has influence on the Forth code: it means that we need
  only one kind of addition and subtraction; only multiplication and
  comparisons come in two version.

  The C standard allows also a representation by sign and magnitude
  and one's complement, but they are rarely used, if at all. And Java
  explicitly requires two's complement, so this may be the future
  quasi-standard. (This includes some guessing, I didn't find yet
  explicit information in the Internet. - mr)


Forth vocabulary
----------------

+ Naming conventions:

 * Normal Forth words are in lower case.
    
   This makes it much easier to switch between Forth and other
   applications in a window environment.

 * `Create`-like words begin with a capital letter.

 * Control structure words like `IF` and `WHILE` are
   capitalised.
  
   This is a kind of convention in Forth code and it lets them stand
   out.

+ There are no words that handle double integers.

  The C integers are large enough for us. (From [graspForth]_.)

+ Strings are null-terminated, as in C.

  This is better for cooperation with C libraries.
