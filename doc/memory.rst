Memory Operations
-----------------

Memory Access
^^^^^^^^^^^^^

.. word:: @		( addr -- n ) "fetch"

   Fetch the cell at *addr*.

   .. source:: [Forth83]_

.. word:: c@		( addr -- n ) "c-fetch"

   Fetch the byte at *addr*.

   .. source:: [Forth83]_

.. word:: !		( n addr -- ) "store"

   Store one cell at *addr*.

   .. source:: [Forth83]_

.. word:: +!		( n addr -- ) "plus-store"

   Add *n* to the cell at *addr*.

   .. source:: [Forth83]_

.. word:: c!		( n addr -- ) "c-store"

   Store one byte at *addr*.

   .. source:: [Forth83]_

.. word:: malloc	( n -- addr )

      Allocate *n* bytes of memory and return its address. Return 0 if
      the allocation fails.

.. word:: free		( addr -- )

      Free the memory space at *addr*, which must have been allocated
      by :word:`malloc`.

.. word:: cells		( n1 -- n2 )

      Compute the number of bytes used by *n1* cells.

.. word:: cell+		( addr1 -- addr2 )

      Increment the TOS by the size of one cell.

.. word:: cell-		( addr1 -- addr2 )

      Decrement the TOS by the size of one cell.


Strings
^^^^^^^

.. word:: append	( addr char -- addr' )

      Store *char* at *addr* and add 1 to *addr*. This word can be
      used to build incrementally a string in memory.

.. word:: strlen	( addr -- n )

      Return the length of the string starting at *addr*, without the
      trailing 0 byte.

.. word:: strchr	( str char -- addr )

   If *char* is contained in *str*, then return the position of its
   first occurrence. Otherwise return 0.

.. word:: bl		( -- char ) "b-l"

   Code for the "blank" character.

   .. source:: [Forth83]_

.. word:: #eol		( -- char )

      Code for the "end of line" character.

.. word:: #eof		( -- inchar )

      Code for the "end of file" constant (which is *not* a character)

.. word:: whitespace	( -- str )

   Zero-terminated string that contains all the characters that are
   viewed as whitespace by :program:`mind`.
