Input/Output
------------

.. word:: emit		( n -- ) |K|, |83|

   Send the character with number *n* to the output.

.. word:: type		( addr n -- ) |K|, |83|

   Send the sequence of characters of length *n* starting at *addr* to
   the output.

.. word:: puts		( addr -- ) |K|

      Send the zero-terminated string beginning at *addr* to the
      output.

.. word:: gets		( addr n -- str ) |K|

   An interface to the function :c:func:`fgets()` from libc.

   The word reads characters from standard input until a return
   character is encountered or *n* - 1 characters are read. A
   zero-terminated string with these characters is created at *addr*.
   The string contains the terminating end-of-line character, if one
   has been typed.

   If no characters could be read from standard input because the
   standard input is in an end-of-file state, the return value is 0,
   and nothing is written to the buffer at *addr*, not even a
   terminating zero.

.. word:: accept		( addr n -- n' ) |K|

   Read a line from the standard input and store it as a
   zero-terminated string with maximal length *n* in the buffer at
   *addr*. The buffer must therefore be at least *n* + 1 bytes wide. A
   line ends if the maximal number of characters is read, a return
   character is encountered, or the input stream has ended. If a
   return has been typed, it is not part of the string.

   The return value is the number of bytes actually read.

   .. source:: [Reva]_, ANSI. Modified for zero-terminated strings.

.. word:: ."            ( <string>" -- ) |I|, "dot-quote"

   Print a string.

.. word:: .(            |K|, |83|, "dot-paren"

   Print the characters that follow this word in the input file to the
   output, until the next ``)``. The closing bracket is not printed.

.. word:: cr |K|, |83|, "c-r"

   Begin a new output line.

.. word:: uh.		( addr -- ) |K|

      Print the TOS as unsigned hexadecimal number, followed by a space.
