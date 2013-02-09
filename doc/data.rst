Data structures
---------------

Data Definitions
^^^^^^^^^^^^^^^^

.. word:: Create        ( <word> -- )

   Create a new dictionary header. A statement :samp:`Create {foo}`
   creates a new header for a word :word:`foo`. When :word:`foo` is
   executed, it returns the address of the first cell behind its
   header. This address is also the value of :word:`here` after
   executing :word:`Create`.

   It is then possible to allocate space in the dictionary with
   :word:`allot`. A typical use of :word:`Create` is therefore in
   phrases like :samp:`Create {foo} ... allot`.

.. word:: Create,       ( 'interpreter <word> -- ) "create-comma"

   Create an header for the name *word* with the interpreter routine
   *'interpret*. This word performs the basic functionality of
   :word:`Create`.

.. word:: does>         "does"

   Used together with :word:`Create` or a word that calls it. The
   typical use is in a definition of the form :samp:`: {Foo} Create
   ... does> ... ;`.

.. word:: Variable      ( <word> -- )

   :samp:`Variable {xxx}` creates a word :word:`xxx` with signature
   ``( -- addr )``, where *addr* is the address of a newly allocated
   cell in the dictionary.

.. word:: Constant      ( n <word> -- )

   :samp:`n Constant {xxx}` creates a word :word:`xxx` with signature
   `( -- n )`.

.. word:: Alias         ( xt <word> -- )

   Define a new word `word` that executes the word with the execution
   token *xt*. It is typically used in the form :samp:`' {foo} Alias
   {bar}`, which defines *bar* as a new name for *foo*.

.. word:: Defer         ( <word> -- )

   Like :word:`Alias`, except that the new word has not yet a defined
   action. It must be later be set with :word:`is`. Before this has
   been done, the execution of the newly defined word causes an error.
   The word is typically used in the definition of mutually recursive
   words.

.. word:: is            ( xt <word> -- )

   Change the activity of a deferred word. *word* must be defined with
   :word:`Alias` or :word:`Defer`. The typical use is then :samp:`'
   {foo} is {bar}`, which makes *foo* the new activity of *bar*.


Dictionary Structure
^^^^^^^^^^^^^^^^^^^^

The internal structure of the dictionary.

.. word:: ^docol |K|

.. word:: ^dodefer |K|

.. word:: ^dovar |K|

.. word:: ^dodoes |K|

.. word:: link>         |K|, "from-link"

   .. source:: [Forth83]_

.. word:: flags@ |K|

.. word:: flags! |K|

.. word:: >name         |K|, "to-name"

   .. source:: [Forth83]_

.. word:: >doer |K|

.. word:: #immediate |K|


Text Streams
^^^^^^^^^^^^

Text streams are an abstraction for the input of program text -- both
from a file and from strings stored in memory. They are the basic data
structures for reading program text.

.. word:: >forward	( 'textstream -- addr ) |K|

   The TOS contains the address of a textstream structure: compute the
   address of its :word:`forward` routine. The routine has the
   signature ( *stream* -- ).

.. word:: >current@	( 'textstream -- addr ) |K|

   The TOS contains the address of a textstream structure: compute the
   address of its :word:`current@` routine. The routine has the
   signature ( *stream* -- *char* ).

.. word:: >eos		( 'textstream -- addr ) |K|

   The TOS contains the address of a textstream structure: compute the
   address of its :word:`eos` routine. The routine has the signature (
   *stream* -- *bool* ).

.. word:: >#eos		( 'textstream -- addr ) |K|

      	The TOS contains the address of a textstream structure:
      	compute the address of its :word:`eos` field. The field is one
      	cell wide and contains the "end of stream" constant of this
      	textstream.

.. word:: >line#	( 'textstream -- addr ) |K|

      	The TOS contains the address of a textstream structure:
      	compute the address of its :word:`line#` field. The field is
      	one cell wide and contains the current line number of this
      	textstream.

.. word:: /textstream	( -- n ) |K|

         	Number of bytes in a text stream structure.

.. word:: 'instream	( -- addr ) |K|

	Variable that contains the address of the current text stream.


File Streams
^^^^^^^^^^^^

A file stream is an extension of the text stream interface for reading
from a file (or any other stream in a Unix system).

A file stream contains all the fields of a text stream, plus
:word:`>intext-file`.

.. word:: >intext-file	( 'filestream -- addr ) |K|

   The TOS contains the address of a filestream structure: compute the
   address of its :word:`>file` field. The field is one cell wide and
   contains the underlying C file pointer :c:type:`FILE*` for this
   stream.

.. word:: >current	( 'filestream -- addr ) |K|

   The TOS contains the address of a filestream structure: compute the
   address of its :word:`>current` field. This field is one cell wide
   and contains the last character read from the file or the "end of
   file" constant.

.. word:: /filestream	( -- n ) |K|

      	Number of bytes in a file stream structure.

.. word:: file-forward	( stream -- ) |K|

   Read one character from a file stream and store it in the
   :word:`>current` field. :word:`line#` is updated if the character
   is an "end of line" symbol.

.. word:: file-current@	( stream -- char ) |K|

	Put the character at the current position of the file stream
	onto the stack.

.. word:: file-eof	( stream -- flag ) |K|

      	Test whether the end of the file stream is reached.

.. word:: forward	( stream -- ) |K|

      Read one character from the current stream. :word:`line#` is
      updated if the character is an "end of line" symbol.

.. word:: current@	( stream -- char ) |K|

      Put the character at the current position of the current stream
      onto the stack.

.. word:: eos		( -- flag ) |K|

      Test whether the end of the current stream is reached.

.. word:: #eos		( -- inchar ) |K|

      The "end of stream" constant for the current stream.

.. word:: line#		( -- addr ) |K|

      Address of the current line number in the current stream. The
      first line of a file has the number 1.

.. word:: do-stream |K|

      Execute the code in the current input stream.
