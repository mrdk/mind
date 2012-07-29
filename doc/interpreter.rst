Interpreter
-----------

Compiler Words
^^^^^^^^^^^^^^

.. word:: [ 		|I|, "left-bracket"

   Switch the interpreter to interpreting mode. All words are now
   executed.

   .. source:: [Forth83]_


Interpreter Words
^^^^^^^^^^^^^^^^^

These words are about reading and interpreting program text.

.. word:: ]             "right-bracket"

   Switch the interpreter to compiling mode. All words are now
   compiled, except for those that are immediate.

   .. source:: [Forth83]_

.. word:: skip-whitespace

      Read from the current stream until the character at the current
      position is no longer an element of :word:`whitespace`. If this
      is already the case, then do nothing.

.. word:: state		( -- addr )

   State of the compiler. If the value is zero, all words are
   interpreted; if it is nonzero, words are compiled and only those
   with an immediate flag are executed.

   .. source:: [Forth83]_

.. word:: (interpret)	( addr -- )

      Search the string at *addr* in the dictionary and interpret or
      compile it, depending on the value of :word:`state`.

.. word:: interpret

   Read one word from the input and execute or compile it, depending
   on the the value of :word:`state`.

   .. source:: [Forth83]_, but different behaviour.

.. word:: notfound

      This word is called by default if a word is not found in the
      dictionary. It prints an error message and closes the input
      file. The word that was searched for is expected as a string at
      :word:`here`.

      This word is the value of :word:`word?` at startup.

.. word:: word?		( -- addr )

      Contains a word that is executed when a string that cannot be
      found in the dictionary by :word:`(interpret)`. At system
      start, its value is :word:`notfound`.

      The Forth word stored in :word:`word?` has the signature `( --
      )`; it expects the searched string at :word:`here`.


.. word:: parse-to	( addr str -- )

      Read a character sequence from the input stream and store it as
      a zero-terminated string at *addr*. The character sequence
      consists of characters not contained in the zero-terminated
      string at *str*. After reading, the reading position in the
      input stream is directly after the first character not contained
      in *str*.

.. word:: parse		( -- addr )

      Read a whitespace-terminated word from the input stream and
      return its address. Afterwards, the reading position in the
      input stream is directly behind the first space character
      *after* that word.

      Currently the parsed word is located directly at the end of the
      dictionary.

.. word:: (') 		( -- cfa )

      Read a word from the input and return its CFA. If it is not
      found, return 0.

.. word:: (find)	( addr -- cfa )

      Search the string at addr in the dictionary and return its CFA.
      If it is not found, return 0.

.. word:: \\ 		|I|

      Start a comment that reaches to the end of the line.

.. word:: ( 		|I|, "paren"

   Start a comment that reaches to the next ``)`` symbol or to the end
   of the page. Note that brackets are not nested.

   .. source:: [Forth83]_


Dictionary
^^^^^^^^^^

These are words to build data structures in the dictionary.

.. word:: align

      Increment (if necessary) the content of :word:`dp` so that it
      points to a valid address for a cell.

.. word:: allot		( n -- )

   Allocate *n* bytes at the end of the dictionary. (Afterwards it
   may be no longer aligned.

   .. source:: [Forth83]_

.. word:: ,		( n -- ) "comma"

   Align the dictionary and put the cell n at its end.

   .. source:: [Forth83]_

.. word:: c,		( b -- ) "c-comma"

   Put the byte b at the end of the dictionary.

   .. source:: [Forth83]_

.. word:: ,"

   Read until the next ``"`` char and put the resulting string at the
   end of the dictionary. The space character immediately after the
   word does not belong to the string.

.. word:: entry,	( str addr -- )

   Put a new entry at the end of the end of the dictionary. *str* is
   its name and *addr* is stored in its CFA field.

.. word:: latest	( -- addr )

      Variable for the address of the latest dictionary entry.

.. word:: dp		( -- addr )

      Dictionary Pointer. It contains the endpoint of the dictionary.

.. word:: here		( -- addr )

   Put the current value of the dictionary pointer onto the stack.

   .. source:: [Forth83]_
