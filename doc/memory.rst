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

   Use the cell at *addr* as a binary flag. :word:`on` stores
   :word:`true` at *addr* and :word:`off` stores :word:`false`.

.. word:: malloc	( n -- addr ) |K|

      Allocate *n* bytes of memory and return its address. Return 0 if
      the allocation fails.

.. word:: free		( addr -- ) |K|

      Free the memory space at *addr*, which must have been allocated
      by :word:`malloc`.

.. word:: cells		( n1 -- n2 ) |K|

      Compute the number of bytes used by *n1* cells.

.. word:: cell+		( addr1 -- addr2 ) |K|, "cell-plus"

      Increment the TOS by the size of one cell.

.. word:: cell-		( addr1 -- addr2 ) |K|, "cell-minus"

      Decrement the TOS by the size of one cell.


Strings
^^^^^^^

.. word:: "             ( <string>" -- ) |I|, "quote"
          
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

.. word:: bl		( -- char ) |K|, |83|, "b-l"

   Code for the "blank" character.

.. word:: #eol		( -- char ) |K|, "number-e-o-l"

      Code for the "end of line" character.

.. word:: #eof		( -- n ) |K|, "number-e-o-f"

      Code for the "end of file" constant (which is *not* a character)

.. word:: whitespace	( -- str ) |K|

   Zero-terminated string that contains all the characters that are
   viewed as whitespace by :program:`mind`.
