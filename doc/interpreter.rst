Interpreter
-----------

Compiler Words
^^^^^^^^^^^^^^

.. word:: [ 		|I|, |K|, |83|, "left-bracket"

   Switch the interpreter to interpreting mode. All words are now
   executed.

.. word:: \:            ( <word> -- cf1 ) |83|, "colon"
          ;             ( cf2 -- ) |83|, |I|, "semicolon"

   Words for the "colon definition" of further Forth words. A typical
   colon definition has the form :samp:`: {name} ... ;`, and it
   defines a new Forth word with the name *name*.

   If the values of *cf1* and *cf2* are different, an error is raised.

.. word:: \:,           ( <word> -- ) "colon-comma"

   Create a header for the name *word* with the interpreter routine of
   `^docol`. This word performs the basic functionality of
   word:`:`. A colon definition ::

       : foo    ... ;

   can then also be written as ::

       :, foo   ] ...  ;; [

.. word:: immediate

   Set the *immediate*-flag for the most recently defined word.
   Afterwards, this word is executed even during a compliation.

.. word:: (")           ( -- addr ) "paren-quote"
          (.")          "paren-dot-quote"
          (abort")      "paren-abort"

   Words that are compiled by words that use an inlined string, like
   `"`, `."` and `abort"`. In the compiled code they
   are followed by a pointer to the address after the string and then
   by the string itself.


Interpreter Words
^^^^^^^^^^^^^^^^^

These words are about reading and interpreting program text.

.. word:: ]             |K|, |83|, "right-bracket"

   Switch the interpreter to compiling mode. All words are now
   compiled, except for those that are immediate.

.. word:: state		( -- addr ) |83|, |K|

   State of the compiler. If the value is zero, all words are
   interpreted; if it is nonzero, words are compiled and only those
   with an immediate flag are executed.

.. word:: exec/compile	( xt -- ) |K|, "exec-compile"

   Interpret or compile the word given by *xt*, depending on the value
   of `state`.

.. word:: interpret |K|

   Read one word from the input and execute or compile it, depending
   on the the value of `state`. (The word occurs in [Forth83]_,
   but with different behaviour.)

.. word:: notfound |K|

      This word is called by default if a word is not found in the
      dictionary. It prints an error message and closes the input
      file. The word that was searched for is expected as a string at
      `here`.

      This word is the value of `word?` at startup.

.. word:: word?		( -- addr ) |K|, |rt|, "word-question"

      Contains a word that is executed when a string that cannot be
      found in the dictionary by `(interpret)`. At system
      start, its value is `notfound`.

      The Forth word stored in `word?` has the signature
      :stack:`( -- )`; it expects the searched string at `here`.

.. word:: skip-whitespace  |K|

   Read from the current stream (which is not necessarily the current
   file) until the character at the current position is no longer an
   element of `whitespace`. If this is already the case, do nothing.

.. word:: parse-to	( addr str -- ) |K|

      Read a character sequence from the input stream and store it as
      a zero-terminated string at *addr*. The character sequence
      consists of characters not contained in the zero-terminated
      string at *str*. After reading, the reading position in the
      input stream is directly after the first character not contained
      in *str*.

.. word:: parse		( -- addr ) |K|

      Read a whitespace-terminated word from the input stream and
      return its address. Afterwards, the reading position in the
      input stream is directly behind the first space character
      *after* that word.

      Currently the parsed word is located directly at the end of the
      dictionary.

.. word:: (') 		( <word> -- xt | 0 ) |vf|, "paren-tick"

   Read a word from the input and return its XT. If it is not found,
   return 0.

.. word:: ' 		( <word> -- xt ) |83|, "tick"

   Read a word from the input and return its XT. If it is not found,
   an error is raised.

.. word:: find          ( str -- xt | 0 ) |K|, |rv|

   Search the string *str* in the dictionary and return its XT. If it
   is not found, return 0.

.. word:: find-word     ( str ctx -- xt | 0 ) |K|, |rv|

   Search the string *str* in the context *ctx* and return its XT. If
   it is not found, return 0.

.. word:: \\ 		|I|, |K|, |vf|, "skip-line"

   Start of a comment that reaches to the end of the line.

.. word:: ( 		|I|, |K|, |83|, "paren"

   Start a comment that reaches to the next ")" symbol or to the end
   of the file. Note that brackets are not nested.


Dictionary
^^^^^^^^^^

These are words to build data structures in the dictionary.

.. word:: align         |K|

      Increment (if necessary) the content of `dp` so that it
      points to a valid address for a cell.

.. word:: allot		( n -- ) |K|, |83|

   Allocate *n* bytes at the end of the dictionary. (Afterwards it
   may be no longer aligned.

.. word:: ,		( n -- ) |K|, |83|, "comma"

   Align the dictionary and put the cell n at its end.

.. word:: c,		( b -- ) |K|, |83|, "c-comma"

   Put the byte b at the end of the dictionary.

.. word:: ,"            ( <string"> -- ) |vf|, "comma-quote"

   Read until the next ``"`` char and put the resulting string at the
   end of the dictionary. The space character immediately after the
   word does not belong to the string.

.. word:: entry,	( str addr -- ) |K|, "entry-comma"

   Put a new entry at the end of the end of the dictionary. *str* is
   its name and *addr* is stored in its XT field.

.. word:: last          ( -- addr ) |K|, |83|

   Variable for the address of the newest dictionary entry.

.. word:: dp		( -- addr ) |K|, |vf|, "d-p"

      Dictionary Pointer. It contains the endpoint of the dictionary.

.. word:: here		( -- addr ) |K|, |83|

   Put the current value of the dictionary pointer onto the stack.
