Data structures
---------------

Dictionary Structure
^^^^^^^^^^^^^^^^^^^^

The internal structure of the dictionary.

.. word:: ^docol

.. word:: ^dodefer

.. word:: ^dovar

.. word:: ^dodoes

.. word:: link>        "from-link"

   .. source:: [Forth83]_

.. word:: flags@

.. word:: flags!

.. word:: >name         "to-name"

   .. source:: [Forth83]_

.. word:: >doer

.. word:: #immediate


Text Streams
^^^^^^^^^^^^

Text streams are an abstraction for the input of program text -- both
from a file and from strings stored in memory. They are the basic data
structures for reading program text.

.. word:: >forward	( 'textstream -- addr )

   The TOS contains the address of a textstream structure: compute the
   address of its :word:`forward` routine. The routine has the
   signature ( *stream* -- ).

.. word:: >current@	( 'textstream -- addr )

   The TOS contains the address of a textstream structure: compute the
   address of its :word:`current@` routine. The routine has the
   signature ( *stream* -- *char* ).

.. word:: >eos		( 'textstream -- addr )

   The TOS contains the address of a textstream structure: compute the
   address of its :word:`eos` routine. The routine has the signature (
   *stream* -- *bool* ).

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


File Streams
^^^^^^^^^^^^

A file stream is an extension of the text stream interface for reading
from a file (or any other stream in a Unix system).

A file stream contains all the fields of a text stream, plus
:word:`>intext-file`.

.. word:: >intext-file	( 'filestream -- addr )

   The TOS contains the address of a filestream structure: compute the
   address of its :word:`>file` field. The field is one cell wide and
   contains the underlying C file pointer :c:type:`FILE*` for this
   stream.

.. word:: >current	( 'filestream -- addr )

   The TOS contains the address of a filestream structure: compute the
   address of its :word:`>current` field. This field is one cell wide
   and contains the last character read from the file or the "end of
   file" constant.

.. word:: /filestream	( -- n )

      	Number of bytes in a file stream structure.

.. word:: file-forward	( stream -- )

   Read one character from a file stream and store it in the
   :word:`>current` field. :word:`line#` is updated if the character
   is an "end of line" symbol.

.. word:: file-current@	( stream -- char )

	Put the character at the current position of the file stream
	onto the stack.

.. word:: file-eof	( stream -- flag )

      	Test whether the end of the file stream is reached.

.. word:: forward	( stream -- )

      Read one character from the current stream. :word:`line#` is
      updated if the character is an "end of line" symbol.

.. word:: current@	( stream -- char )

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
