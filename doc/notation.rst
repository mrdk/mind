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

   *cfa*
	"Code Field Address": The address of a word that can be
	executed.

   *n*
	a number, or sometimes an arbitrary value

   *a*, *b*, *c*
	arbitrary values.


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

