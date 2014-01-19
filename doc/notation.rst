Notation
========

Stack Diagrams
--------------

A stack diagram looks like :stack:`( n1 n2 n3 -- n4 n5)`. The left
side of the diagram shows the top entries before the execution of the
word, the right side afterwards. On both sides, the top of the stack
appears at the right.

Sometimes more than one stack is involved. Then the stack diagram has
several sections, separated by a vertical bar. Each section has a
prefix that specifies the stack to which the diagram refers. The most
important other stack is the :ref:`return stack <return-stack>`. Its
prefix is *R:*. An example is the word `>r`. It transfers a value from
the parameter stack to the return stack and has the diagram :stack:`(n
-- | R: -- n)`.

There are standard abbreviations for the contents of stack cells. This
is a list of the most important ones.

*addr*
    an address in memory.

*char*
    a text character: a number that fits into one byte.

*flag*
    a boolean flag, either `true` or `false`.

*cf*
    a "compilation flag". These flags are left on the stack during the
    compilation of control stuctures (like ``IF ... ELSE ... THEN``)
    in order to check for correct nesting.

*str*
    the start address of an zero-terminated string

*xt*
    "Execution Token": The address of a word that can be executed.

*n*
    a number, or sometimes an arbitrary value

*a*, *b*, *c*
    arbitrary values.

*<word>*
    a word that is read from the input stream. Words are sequences
    of nonblank characters.


Naming Conventions
------------------

To keep the names of words short, many naming conventions are used in
the Forth community. *mind* follows mainly those listed in
[ThinkingForth]_, p. 288--290, but also adds some of its own.

.. describe:: Xxxx

   Caitalised words create other words, usually by calling `Create`.
   They are typically used in the form :samp:`Xxxx {foo}`. Such a call
   is expected to create a word *foo*.

.. describe:: XXX

   Uppercase words influence the control flow. Examples are `BEGIN` or
   `IF`.

.. describe:: xxx!

   Words that store stack values in memory, like `c!`. The value on
   the stack does not need to be copied literally, e.g. in `+!`.

.. describe:: xxx"

   A word that must be followed by a string literal in the source
   code, like `abort"`

.. describe:: #xxx

   A numerical constant, often "number of *xxx*".

.. describe:: 'xxx

   Return an address that is related to the word *xxx*, as in `'`.

.. describe:: (xxx)

   A word that provides inplementation details for *xxx*, like `(.)`.
   In many cases it should not be called directly.

.. describe:: xxx,

   Add data to the end of the dictionary, like `c,`.

.. describe:: .xxx

   Print something to the standard output, like `.` or `u.`.

.. describe:: /xxx

   The size of a data type, like `/cell`. The notation is an
   abbreviation for "bytes per *xxx*".

.. describe:: ?xxx

   Do an action conditionally, like `?dup`.

.. describe:: xxx?

   Return a boolean flag, like `i?`.

.. describe:: xxx@

   Copy a memory value to the stack, like `c@`.

.. describe:: [xxx]

   Run at compile time, like `[char]`. In contrast to uppercase words,
   these words do not influence control flow.


Dictionary Flags
----------------

The following abbreviations are used in the header lines for forth words.

.. Abbreviation: |I|, defined in conf.py

.. _immediate:

*I* -- immediate
    The word is always executed, even during compilation. It has the
    "immediate" flag set in its header.

.. Abbreviation: |K|, defined in conf.py

.. _kernel:

*K* -- kernel
    The word is defined in the kernel of :program:`mind`.

.. Abbreviation: |83|, defined in conf.py

.. _from-forth83:

*83* -- Forth-83
     The word is defined in the [Forth83]_ standard.

.. Abbreviation: |vf|, defined in conf.py

.. _from-volksforth:

*vf* -- volksForth
     The word is defined in [volksForth]_ and not already part of
     [Forth83]_.

.. Abbreviation: |rt|, defined in conf.py

.. _from-retro:

*rt* -- retro
     The word is part of [Retro]_.

.. Abbreviation: |rv|, defined in conf.py

.. _from-reva:

*rv* -- Reva
     The word is part of [Reva]_.
