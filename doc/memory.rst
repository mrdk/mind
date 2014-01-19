Memory Operations
-----------------

Memory Access
^^^^^^^^^^^^^

.. word:: @		( addr -- n ) |K|, |83|, "fetch"

   Fetch the cell at *addr*.

.. word:: !		( n addr -- ) |K|, |83|, "store"

   Store one cell at *addr*.

.. word:: +!		( n addr -- ) |K|, |83|, "plus-store"

   Add *n* to the cell at *addr*.

.. word:: c@		( addr -- n ) |K|, |83|, "c-fetch"

   Fetch the byte at *addr*.

.. word:: c!		( n addr -- ) |K|, |83|, "c-store"

   Store one byte at *addr*.

.. word:: on            ( addr -- )
          off           ( addr -- )

   Use the cell at *addr* as a binary flag. `on` stores
   `true` at *addr* and `off` stores `false`.

.. word:: cmove         ( from to u -- ) |K|, |83|, "c-move"

   Copy the *u* bytes beginning with the address *from* to the *u*
   byte long region beginning at the address *to*. The two regions may
   not overlap.

.. word:: fill          ( addr u char -- ) |K|, |83|

   Fill the *u* bytes starting at *addr* with the byte *char*.

.. word:: erase         ( addr u -- ) |83|

   Fill the *u* bytes starting at *addr* with zeros.

.. word:: malloc	( n -- addr ) |K|

      Allocate *n* bytes of memory and return its address. Return 0 if
      the allocation fails.

.. word:: free		( addr -- ) |K|

      Free the memory space at *addr*, which must have been allocated
      by `malloc`.

.. word:: /cell         ( -- n ) |K|, "per-cell"

   Return the number of bytes per cell.

.. word:: cells		( n1 -- n2 )

      Compute the number of bytes used by *n1* cells.

.. word:: cell+		( addr1 -- addr2 ) |K|, "cell-plus"

      Increment the TOS by the size of one cell.

.. word:: cell-		( addr1 -- addr2 ) |K|, "cell-minus"

      Decrement the TOS by the size of one cell.


Characters and Strings
^^^^^^^^^^^^^^^^^^^^^^

.. word:: char          ( <word> -- char )

   Return the number of the character at the beginning of the
   following word. ``char A`` returns therefore the number 65.

.. word:: [char]        ( -- char | Compile: <word> -- ) |I|, "bracket-compile"

   Compilation word. Puts the number for the character at the
   beginning of *<word>* as literal into the code. Writing ``[char]
   A`` in the body of a colon definition has therefore the same effect
   as writing ``65``.

.. word:: bl		( -- char ) |K|, |83|, "b-l"

   Code for the "blank" character.

.. word:: #eol		( -- char ) |K|, |vf| "number-e-o-l"

      Code for the "end of line" character.

.. word:: #eof		( -- n ) |K|, "number-e-o-f"

   Code for the "end of file" constant (which is *not* a character).

.. word:: "             ( <string"> -- ) |I|, "quote"
          
   String literal.

.. word:: append	( addr char -- addr' ) |K|

      Store *char* at *addr* and add 1 to *addr*. This word can be
      used to build incrementally a string in memory.

.. word:: strlen	( addr -- n ) |K|

      Return the length of the string starting at *addr*, without the
      trailing 0 byte.

.. word:: strchr	( str char -- addr ) |K|

   If *char* is contained in *str*, then return the position of its
   first occurrence. Otherwise return 0.

.. word:: whitespace	( -- str ) |K|

   Zero-terminated string that contains all the characters that are
   viewed as whitespace by :program:`mind`.
