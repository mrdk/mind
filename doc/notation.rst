Notation
========

Abbreviations in Stack Diagrams
-------------------------------

*addr*
    an address in memory.

*char*
    a text character: a number that fits into one byte.

*flag*
    a boolean flag, either 0 or -1.

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

