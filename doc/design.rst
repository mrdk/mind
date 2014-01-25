Design decisions
================

C Code
------

+ The C code for the :program:`mind` kernel is written for
  :program:`gcc`.

  This is because :program:`mind` should become as portable as
  possible. It will then be able to run under all systems supported by
  :program:`gcc`. The restriction to :program:`gcc` is necessary
  because :program:`mind` relies on the computed **goto**, which is a
  non-standard :program:`gcc` extension.

+ The program uses C99 as C standard. Features of C11 are used only as
  far as necessary, if :program:`gcc` supports them (possibly with a
  different syntax).

  Again, the goal is portability. C99 appears to be well-supported
  now.

  Currently the only example is the function :c:func:`__alignof`. It
  is part of C11, in the form :c:func:`_Alignof`.

+ The kernel of :program:`mind` should rely only on standard C
  libraries, including POSIX.

  The ultimate goal is a portable system that runs on all systems that
  are supported by :program:`gcc`. For faster development it is
  however better to have many libraries at one's disposal. The C
  standard libraries, together with POSIX, provide a comfortable basis
  for system programming.

  Currently, :program:`mind` uses:

  - The C standard libraries, as far as they are included in the C99
    standard.

  - POSIX libraries: All POSIX.1-2001 functions are used, together
    with those functions in POSIX.1-2008 that could also be
    implemented by hand if necessary.

+ A cell may contain both an :c:type:`int` and a pointer.
  
  The basic Forth data type, the cell, becomes the smallest integer
  type with at least as many bytes to store both an :c:type:`int` or a
  :c:type:`void*`.

  This will make it possible to interface to C functions in a machine
  independent and efficient way: It will always be possible to store
  an :c:type:`int`, a :c:type:`char` or any pointer on the stack.

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

+ Normal Forth words are in lower case.
    
  This makes it much easier to switch between Forth and other
  applications in a windowed environment.

+ :program:`mind` uses the common Forth naming conventions.

  These are a well-developed system to keep the programs short and
  expressive. For the naming conventions, see
  :ref:`naming-conventions`.

+ There are no words that handle double integers. (From
  [graspForth]_.)

  The C integers are large enough especially on 64-bit systems.

+ Strings are null-terminated, as in C.

  This is better for cooperation with C libraries.
