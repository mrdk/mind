Input/Output
------------

Strings
^^^^^^^

.. word:: emit		( n -- ) |K|, |83|

   Send the character with number *n* to the output.

.. word:: space         |83|

   Send a blank character to the output

.. word:: spaces        ( n -- ) |83|

   Send *n* spaces to the output. If *n* is negative, do nothing.

.. word:: cr            |K|, |83|, "c-r"

   Begin a new output line.

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

.. word:: accept        ( addr n -- n' ) |K|

   Read a line from the standard input and store it as a
   zero-terminated string with maximal length *n* in the buffer at
   *addr*. The buffer must therefore be at least *n* + 1 bytes wide. A
   line ends if the maximal number of characters is read, a return
   character is encountered, or the input stream has ended. If a
   return has been typed, it is not part of the string.

   The return value is the number of bytes actually read.

.. word:: ."            ( <string>" -- ) |I|, "dot-quote"

   Print a string.

.. word:: .(            |K|, |83|, "dot-paren"

   Print the characters that follow this word in the input file to the
   output, until the next ``)``. The closing bracket is not printed.


Numbers
^^^^^^^

.. word:: base          ( -- addr ) |83|

   Variable that contains the base for number conversion. The minimal
   value of `base` is 2, the maximal value is 36. Most words
   that convert a number from its internal representation as a cell to
   a string or backwards use this variable.

.. word:: binary        |vf|
          octal         |83|
          decimal       |83|
          hex           |83|

   Sets `base` to 2, 8, 10 or 16, respectively.

.. word:: .             ( n -- ) |83|, "dot"
          u\.           ( u -- ) |83|, "u-dot"

   Print the TOS as a signed or unsigned number, followed by a space.
   The conversion uses the value of `base`.

.. word:: (.)           ( n -- str ) "paren-dot"
          (u.)          ( u -- str ) "paren-u-dot"

   Return the address that contains the TOS as a signed or unsigned
   number, according to `base`. There is no trailing space here.
   The string exists until another word is called that converts a cell
   to a string, then it is overwritten. (Among these words are also
   `.` and `h.` and others, since they use internally
   `(.)` and `(u.)`.)

.. word:: h.            ( n -- ) |83|, "h-dot"
          uh.           ( u -- ) |K|, "u-h-dot"

   Print the TOS as a signed or unsigned hexadecimal number, followed
   by a space. The value of `base` is unchanged.
