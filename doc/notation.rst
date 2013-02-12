Notation
========

Abbreviations in Stack Diagrams
-------------------------------

   *addr*
        an address in memory.

   *char*
        a text character: a number that fits into one byte.

   *inchar*
        an "input character": either a *char* or the constant
        :word:`#eof`.

   *flag*
	a boolean flag, either 0 or -1.

   *str*
	the start address of an zero-terminated string

   *xt*
	"Execution Token": The address of a word that can be
	executed.

   *n*
	a number, or sometimes an arbitrary value

   *a*, *b*, *c*
	arbitrary values.

   *<word>*
        a word that is read from the input stream. Words are sequences
        of nonblank characters.


Flags
-----

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
