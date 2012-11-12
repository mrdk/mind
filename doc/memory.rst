Memory Operations
-----------------

Memory Access
^^^^^^^^^^^^^

.. word:: @		( addr -- n ) |K|, "fetch"

   Fetch the cell at *addr*.

   .. source:: [Forth83]_

.. word:: c@		( addr -- n ) |K|, "c-fetch"

   Fetch the byte at *addr*.

   .. source:: [Forth83]_

.. word:: !		( n addr -- ) |K|, "store"

   Store one cell at *addr*.

   .. source:: [Forth83]_

.. word:: +!		( n addr -- ) |K|, "plus-store"

   Add *n* to the cell at *addr*.

   .. source:: [Forth83]_

.. word:: c!		( n addr -- ) |K|, "c-store"

   Store one byte at *addr*.

   .. source:: [Forth83]_

.. word:: malloc	( n -- addr ) |K|

      Allocate *n* bytes of memory and return its address. Return 0 if
      the allocation fails.

.. word:: free		( addr -- ) |K|

      Free the memory space at *addr*, which must have been allocated
      by :word:`malloc`.

.. word:: cells		( n1 -- n2 ) |K|

      Compute the number of bytes used by *n1* cells.

.. word:: cell+		( addr1 -- addr2 ) |K|

      Increment the TOS by the size of one cell.

.. word:: cell-		( addr1 -- addr2 ) |K|

      Decrement the TOS by the size of one cell.


Strings
^^^^^^^

.. word:: append	( addr char -- addr' ) |K|

      Store *char* at *addr* and add 1 to *addr*. This word can be
      used to build incrementally a string in memory.

.. word:: strlen	( addr -- n ) |K|

      Return the length of the string starting at *addr*, without the
      trailing 0 byte.

.. word:: strchr	( str char -- addr ) |K|

   If *char* is contained in *str*, then return the position of its
   first occurrence. Otherwise return 0.

.. word:: bl		( -- char ) |K|, "b-l"

   Code for the "blank" character.

   .. source:: [Forth83]_

.. word:: #eol		( -- char ) |K|

      Code for the "end of line" character.

.. word:: #eof		( -- inchar ) |K|

      Code for the "end of file" constant (which is *not* a character)

.. word:: whitespace	( -- str ) |K|

   Zero-terminated string that contains all the characters that are
   viewed as whitespace by :program:`mind`.