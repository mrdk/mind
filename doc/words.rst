Vocabulary
==========

Abbreviations in stack diagrams
-------------------------------

   *addr*
        an address in memory.

   *char*
        a text character: a number that fits into one byte.

   *inchar*
        an "input characte": either a *char* or the constant
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
	arbitray values.


Glossary
--------

.. glossary::

   cell 
        A unit of memory large enough to store an integer or an
        address.
  
   CFA
        Code Field Address. This is the address of a word such that
        it can be executed.

   flag
        A boolean value. If a word returns a flag, it is either 0, for
        "false", or -1, for true. If a word accepts a flag, every
        value different from 0 is interpreted as "true".

   TOS
        Top of Stack.

   word
        (a) a Forth procedure,

	(b) a sequence of non-space characters surrounded by spaces.

   variable
        A memory region to store the value of one cell (unless another
        size is specified). If a word is called a variable, it returns
        the address of this region.

Tags
----

 - immediate

    The word is always executed, even during compilation
    It has the "immediate" flag set in its header.

Kernel words
============

Starting and Ending
-------------------

.. word:: boot

      Initialise the system completely and start the outer
      interpreter.

.. word:: abort

      Stop the interpretation of the currently read text stream and
      return to the interactive mode by executing :word:`'abort`.

.. word:: 'abort		( -- addr )

      Variable that contains a word that does is called after an error
      has occured; it is expected to reset the parameter stack and the
      return stack and then to start an interactive prompt.

.. word:: bye

      Leave the Forth system immediately.

Inner Interpreter
-----------------

.. word:: noop

      Do nothing.

.. word:: ;;

      End the execution of the current word.

      .. source:: retroforth

.. word:: if;		( n -- )

      Leave the execution of the current word if the TOS is nonzero.

      .. source:: retroforth

.. word:: 0; 		( 0 -- | n -- n )

      If the TOS is zero, drop it and end the execution of the current
      word.

      .. source:: retroforth

.. word:: execute		( addr -- )

      Execute the word at addr.

Outer Interpreter
-----------------

    These words are about reading and interpreting program text.

.. word:: (interpret)	( addr -- )

      Search the string at *addr* in the dictionary and interpret or
      compile it, depending on the value of :word:`state`.

.. word:: interpret

      Read one word from the input and execute or compile it,
      depending on the the value of :word:`state`.

.. word:: word?		( -- addr )

      Contains a word that is executed when a string that cannot be
      found in the dictionary by :word:`(interpret)`. At system
      start, its value is :word:`notfound`.

      The Forth word stored in :word:`word` has the signature `--`; it
      expects the searched string at :word:`here`.

.. word:: notfound

      This word is called by default if a word is not found in the
      dictionary. It prints an error message and closes the input
      file. The word that was searched for is expected as a string at
      :word:`here`.

      This word is the value of :word:`word?` at startup.

.. word:: parse-to		( addr str -- )

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

.. word:: \\ 							  :immediate:

      Start a comment that reaches to the end of the line.

.. word:: ( 							  :immediate:

      Start a comment that reaches to the next ")" symbol or to the
      end of the page. Note that brackets are not nested.

Command Line Parameters
-----------------------

    "mind" has the following command line parameters:

    .. option:: -e <cmd>

       Execute <cmd> and finish.
        
    .. option:: -x <cmd>

       Execute <cmd> and start interactive mode.

    .. option:: -h

       Print help text.

    The parameters set the following Forth variables:

.. word:: start-command	( -- addr )

      Variable containing the address of a string that is set by the
      command options :option:`-e` and :option:`-x`; otherwise its
      value is 0.

      If the value of :word:`start-command` is nonzero, then it
      contains a string that is executed after the file "start.mind"
      is read and before the system switches to interactive mode (if
      it does).

.. word:: interactive-mode	( -- addr )

      Variable containing a flag that is set to :word:`false` by the
      command line options :option:`-e`. By default its value is
      :word:`true`.

      If the value of :word:`interactive-mode` is :word:`true`, then
      "mind" switches to an interactive mode after startup.

Text streams
------------

    Text streams are an abstraction for the input of program text --
    both from a file and from strings stored in memory.

Text streams
^^^^^^^^^^^^

      These are the basic data structures for reading program text.

.. word:: >forward	( 'textstream -- addr )

      	The TOS contains the address of a textstream structure:
      	compute the address of its :word:`forward` routine. The
      	routine has the signature ( stream -- ).

.. word:: >current@	( 'textstream -- addr )

      	The TOS contains the address of a textstream structure:
      	compute the address of its :word:`current@` routine. The
      	routine has the signature ( stream -- char ).

.. word:: >eos		( 'textstream -- addr )

      	The TOS contains the address of a textstream structure:
      	compute the address of its :word:`eos` routine. The routine
      	has the signature ( stream -- bool ).

.. word:: >#eos		( 'textstream -- addr )

      	The TOS contains the address of a textstream structure:
      	compute the address of its :word:`eos` field. The field is one
      	cell wide and contains the "end of stream" constant of this
      	textstream.

.. word:: >line#	( 'textstream -- addr )

      	The TOS contains the address of a textstream structure:
      	compute the address of its :word:`line#` field. The field is
      	one cell wide and contains the current line number of this
      	textstream.

.. word:: /textstream	( -- n )

         	Number of bytes in a text stream structure.

.. word:: 'instream	( -- addr )

	Variable that contains the address of the current text stream.

File streams
^^^^^^^^^^^^

      A file stream is an extension of the text stream interface for
      reading from a file (or any other stream in a Unix system).

      A file stream contains all the fields of a text stream, plus
      :word:`intext-file`.

.. word:: >intext-file	( 'filestream -- addr )

      	The TOS contains the address of a filestream structure:
      	compute the address of its :word:`file` field. The field is
      	one cell wide and contains the underlying C file pointer
      	(FILE*) for this stream.

.. word:: >current	( 'filestream -- addr )

      	The TOS contains the address of a filestream structure:
      	compute the address of its `current` field. The field is
      	one cell wide and contains the last character read from the
      	file or the "end of file" constant.

.. word:: /filestream	( -- n )

      	Number of bytes in a file stream structure.

.. word:: file-forward	( stream -- )

      	Read one character from a file stream and store it in the
      	:word:`current` field. :word:`line#` is updated if the
      	character is an "end of line" symbol.

.. word:: file-current@	( stream -- char )

	Put the character at the current position of the file stream
	onto the stack.

.. word:: file-eof	( stream -- flag )

      	Test whether the end of the file stream is reached.

.. word:: forward		( stream -- )

      Read one character from the current stream. :word:`line#` is
      updated if the character is an "end of line" symbol.

.. word:: current@		( stream -- char )

      Put the character at the current position of the current stream
      onto the stack.

.. word:: eos		( -- flag )

      Test whether the end of the current stream is reached.

.. word:: #eos		( -- inchar )

      The "end of stream" constant for the current stream.

.. word:: line#		( -- addr )

      Address of the current line number in the current stream. The
      first line of a file has the number 1.

.. word:: do-stream

      Execute the code in the current input stream.

Compilation
-----------

.. word:: [ 							  :immediate:

      Switch the interpreter to interpreting mode. All words are now
      executed.

.. word:: ]

      Switch the interpreter to compiling mode. All words are now
      compiled, except for those that are immediate.

.. word:: skip-whitespace

      Read from the current stream until the character at the current
      position is no longer an element of :word:`whitespace`. If this
      is already the case, then do nothing.

.. word:: state		( -- addr )

      State of the compiler. If the value is zero, all words are
      interpreted; if it is nonzero, words are compiled and only those
      with an immediate flag are executed.

.. word:: branch

      Unconditional jump. The cell following this word contains the
      address of the jump target.

.. word:: 0branch		( n -- )

      Conditional jump. If *n* is zero, jump to the address in the
      next cell. If *n* is nonzero, continue with the execution of the
      word after the next cell.

.. word:: lit		( -- n )

      Push the content of the cell after this word onto the stack.

Dictionary
----------

.. word:: align

      Increment (if necessary) the content of :word:`dp` so that it
      points to a valid address for a cell.

.. word:: allot		( n -- )

      Allocate n bytes at the end of the dictionary. (Afterwards, it
      may be no longer aligned.

.. word:: ,			( n -- )

      Align the dictionary and put the cell n at its end.

.. word:: c,		( b -- )

      Put the byte b at the end of the dictionary.

.. word:: ,"

      Read until the next `"` and put the resulting string at the end
      of the dictionary. The space character immediately after the
      word does not belong to the string.

.. word:: entry,		( str addr -- )

      Put a new entry at the end of the end of the dictionary. *str*
      is its name and addr is stored in its CFA field.

.. word:: latest		( -- addr )

      Variable for the address of the latest dictionary entry.

.. word:: dp		( -- addr )

      Dictionary Pointer. It contains the endpoint of the dictionary.

.. word:: here		( -- addr )

      Put the current value of the dictionary pointer onto the stack.

.. word:: (') 		( -- cfa )

      Read a word from the input and return its CFA. If it is not
      found, return 0.

.. word:: (find)		( addr -- cfa )

      Search the string at addr in the dictionary and return its CFA.
      If it is not found, return 0.

Dictionary Headers
------------------

.. word:: ^docol

.. word:: ^dodefer

.. word:: ^dovar

.. word:: ^dodoes

.. word:: link>

.. word:: flags@

.. word:: flags!

.. word:: >name

.. word:: >doer

.. word:: #immediate

Return stack
------------

.. word:: rdrop

      Remove the top value of the return stack.

.. word:: >r		( n -- )

      Move the TOS to the top of the return stack.

.. word:: r>		( -- n )

      Move the top of the return stack to the TOS.

.. word:: >rr		( n -- )

      Move the TOS to the second position of the return stack.

      .. source:: Reva

.. word:: rr>		( -- n )

      Move the second entry of the return stack to the TOS.

      .. source:: Reva

.. word:: r@		( -- n )

      Copy the top of the return stack to the TOS.

.. word:: r0		( -- addr )

      Variable for the position of the return stack pointer when the
      stack is empty

Stack
-----

.. word:: drop		( a -- )

.. word:: nip		( a b -- b )

.. word:: 2drop		( a b -- )

.. word:: ?dup		( 0 -- 0 | n -- n n )

      Duplicate the TOS only if it is nonzero

.. word:: dup		( a -- a a )

.. word:: over		( a b -- a b a )

.. word:: under		( a b -- b a b )

.. word:: swap		( a b -- b a )

.. word:: rot		( a b c -- b c a )

.. word:: -rot		( a b c -- c a b )

.. word:: sp@		( -- addr )

      Get the value of the stack pointer. `sp@ @` is equivalent to
      :word:`dup`.

.. word:: sp!		( addr -- )

      Make *addr* the new value of the stack pointer.

.. word:: s0		( -- addr )

      Variable for the position of the stack pointer when the stack is
      empty

Integer Arithmetic
------------------

.. word:: 0			( -- 0 )

      This and other numbers are defined as Forth words to shorten the
      compiled code and to make the bootstrapping of the language
      easier.

.. word:: 1			( -- 1 )

.. word:: -1		( -- -1 )

.. word:: 2			( -- 2 )

.. word:: 1+		( n -- n' )

      Add 1 to the TOS.

.. word:: 1-		( n -- n' )

      Subtract 1 from the TOS.

.. word:: 2*		( n -- n' )

      Multiplication with 2, as signed integer.

.. word:: 2/		( n -- n' )

      Division by 2, as signed integer.

.. word:: -			( n1 n2 -- n3 )

      Compute the difference n1 - n2.

.. word:: +			( n1 n2 -- n3 )

      Compute the sum of n1 and n2.

.. word:: *			( n1 n2 -- n3 )

      Compute the product of n1 and n2

.. word:: /			( n1 n2 -- n3 )

      Compute the quotient n1 / n2 as integer.

      Currently this is C arithmetics, with rounding towards 0. (It
      may be changed later.)

.. word:: mod		( n1 n2 -- n3 )

      Compute n1 mod n2

.. word:: /mod		( n1 n2 -- quot rem )

      *quot* is n1 / n2 and *rem* is n1 mod n2.

.. word:: u*		( n1 n2 -- n3 )

      Product of n1 and n2 as unsigned integers.

.. word:: u/		( n1 n2 -- n3 )

      Quotient of n1 and n2 as unsigned integer.

.. word:: abs		( n -- u )

      Compute the absolute value

Binary Arithmetic
-----------------

.. word:: false		( -- flag )

      Boolean flag for false,

.. word:: true		( -- flag )

      Boolean flag for true.

.. word:: or		( n1 n2 -- n3 )

      Bitwise "or" of n1 and n2.

.. word:: and		( n1 n2 -- n3 )

      Bitwise "and" of n1 and n2.

.. word:: xor		( n1 n2 -- n3 )

      Bitwise exclusive "or" of n1 and n2.

.. word:: invert		( n1 -- n2 )

      Bitwise nagation of the TOS.

Comparisons
-----------

.. word:: =			( n1 n2 -- flag )

      Test whether n1 and n2 are equal.

.. word:: <>		( n1 n2 -- flag )

      Test whether n1 and n2 are unequal.

.. word:: 0=		( n -- flag )

      Test whether the TOS is equal to 0. (This also inverts boolean
      flags.)

.. word:: 0<		( n -- flag )

      Test whether TOS < 0

.. word:: 0>		( n -- flag )

      Test whether TOS > 0

.. word:: <			( n1 n2 -- flag )

      Test whether n1 < n2.

.. word:: <=		( n1 n2 -- flag )

      Test whether n1 <= n2.

.. word:: >			( n1 n2 -- flag )

      Test whether n1 > n2.

.. word:: >=		( n1 n2 -- flag )

      Test whether n1 >= n2.

.. word:: u<		( n1 n2 -- flag )

      Test whether n1 < n2 as unsigned integers.

.. word:: u<=		( n1 n2 -- flag )

      Test whether n1 <= n2 as unsigned integers.

.. word:: u>		( n1 n2 -- flag )

      Test whether n1 > n2 as unsigned integers.

.. word:: u>=		( n1 n2 -- flag )

      Test whether n1 >= n2 as unsigned integers.

.. word:: within		( n n0 n1 -- flag )

      True if n0 <= n <= n1. The sequence of integers is here viewed
      as cyclic; the word works therefore with unsigned integers as
      well as with signed ones.

Memory
------

.. word:: @			( addr -- n )

      Fetch the cell at *addr*.

.. word:: c@		( addr -- n )

      Fetch the byte at *addr*.

.. word:: !			( n addr -- )

      Store one cell at *addr*.

.. word:: +!		( n addr -- )

      Add *n* to the cell at *addr*.

.. word:: c!		( n addr -- )

      Store one byte at *addr*.

.. word:: malloc		( n -- addr )

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
-------

.. word:: append		( addr char -- addr' )

      Store *char* at *addr* and add 1 to *addr*. This word can be
      used to build incrementally a string in memory.

.. word:: strlen		( addr -- n )

      Return the length of the string starting at *addr*, without the
      trailing 0 byte.

.. word:: strchr		( str char -- addr )

      If *char* is contained in *str*, then return the position of its
      first occurrence. Othewise return 0.

.. word:: bl		( -- char )
      Code for the "blank" character.

      .. source:: Forth 83

.. word:: #eol		( -- char )

      Code for the "end of line" character.

.. word:: #eof		( -- inchar )

      Code for the "end of file" constant (which is *not* a character)

.. word:: whitespace	( -- str )

      Zero-terminated string that contains all the characters that are
      viewed as whitespace by Mind.

Input/Output
------------

.. word:: emit		( n -- )

      Send the character with number *n* to the output.

.. word:: type		( addr n -- )

      Send the sequence of characters of length *n* starting at *addr*
      to the output.

.. word:: puts		( addr -- )

      Send the zero-terminated string beginning at *addr* to the
      output.

.. word:: gets		( addr n -- str )

      An interface to the function `fgets()` from libc.

      The word reads characters from standard input until a return
      character is encountered or *n* - 1 characters are read. A
      zero-terminated string with these characters is created at
      *addr*. The string contains the terminating end-of-line
      character, if one has been typed.

      If no characters could be read from standard input because the
      standard input is in an end-of-file state, the return value is
      0, and nothing is written to the buffer at *addr*, not even a
      terminating zero.

.. word:: accept		( addr n -- n' )

      Read a line from the standard input and store it as a
      zero-terminated string with maximal length *n* in the buffer at
      *addr*. The buffer must therefore be at least *n* + 1 bytes
      wide. A line ends if the maximal number of characters is read, a
      return character is encountered, or the input stream has ended.
      If a return has been typed, it is not part of the string.

      The return value is the number of bytes actually read.

      .. source:: Reva, ANSI. Modified for zero-terminated strings.

.. word:: .(

      Print the characters that follow this word in the input file to
      the output, until the next ")". The closing bracket is not
      printed.

.. word:: cr

      Begin a new output line.

.. word:: uh.		( addr -- )

      Print the TOS as unsigned hexadecimal number, followed by a space.


Copyright 2012 Markus Redeker. This work is licensed under the Creative
Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of
this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or
send a letter to Creative Commons, 444 Castro Street, Suite 900,
Mountain View, California, 94041, USA.
