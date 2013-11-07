Data structures
===============

Data Definitions
----------------

.. word:: Create        ( <word> -- ) |83|

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

.. word:: does>         |83|, "does"

   Used together with :word:`Create` or a word that calls it. The
   typical use is in a definition of the form :samp:`: {Foo} Create
   ... does> ... ;`.

.. word:: Variable      ( <word> -- ) |83|

   :samp:`Variable {xxx}` creates a word :word:`xxx` with signature
   ``( -- addr )``, where *addr* is the address of a newly allocated
   cell in the dictionary.

.. word:: Constant      ( n <word> -- ) |83|

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
--------------------

The internal structure of the dictionary.

.. word:: ^docol |K|

.. word:: ^dodefer |K|

.. word:: ^dovar |K|

.. word:: ^dodoes |K|

.. word:: >link         ( xt -- addr ) |K|, |83|, "to-link"

   Convert the execution token of a word to the address of its link
   field. This field is one cell wide and contains the address of the
   link field of the word defined before this -- or 0 of there is no
   previous word.

.. word:: link>         ( addr -- xt ) |K|, |83|, "from-link"

   Convert the address of the link field of a word to its execution
   token.

.. word:: >name         ( xt -- addr ) |K|, |83|, "to-name"

   Convert an execution token to the address of the name field of a
   word. The name field contains the address of the null-terminated
   string that is the name. This means that the command sequence ::

     ' word >name @

   leaves the address of the string `"word"` on the stack.

.. word:: >doer         ( xt -- addr ) |K|, "to-doer"

   Convert the execution token of a word to the address of its doer
   field. In words defined with :word:`does`, it contains the address
   of the code that is eexcuted by them. Otherwise it is usually 0.

.. word:: >body         ( xt -- addr ) |K|, |83|, "to-body"

   Convert the execution token of a word to the address of its body.

.. word:: body>         ( addr -- xt ) |K|, |83|, "from-body"

   Convert the address of the body of a word to its execution token.

.. word:: flags@ |K|

.. word:: flags! |K|

.. word:: #immediate |K|


Streams
-------

Streams are an abstraction for the input from a source that produces
data continuously. There is always one "current stream", with its
address in :word:`'instream`.

.. word:: >forward	( 'stream -- addr ) |K|, "to-forward"

   The TOS contains the address of a stream structure: compute the
   address of its :word:`forward` routine. The routine has the
   signature ( -- ).

.. word:: >current@	( 'stream -- addr ) |K|, "to-current-fetch"

   The TOS contains the address of a stream structure: compute the
   address of its :word:`current@` routine. The routine has the
   signature ( -- *char* ).

.. word:: >eos		( 'stream -- addr ) |K|, "to-e-o-s"

   The TOS contains the address of a stream structure: compute the
   address of its :word:`eos` routine. The routine has the signature (
   -- *bool* ).

.. word:: /stream	( -- n ) |K|, "per-stream"

   Number of bytes in a stream data structure.

.. word:: 'instream	( -- addr ) |K|, "tick-instream"

   Variable that contains the address of the current stream.

.. word:: forward	( -- ) |K|

      Read one character from the current stream. :word:`line#` is
      updated if the character is an "end of line" symbol.

.. word:: current@	( -- char ) |K|, "current-fetch"

      Put the character at the current position of the current stream
      onto the stack.

.. word:: eos		( -- flag ) |K|, "e-o-s"

      Test whether the end of the current stream is reached.


File Streams
------------

A file stream is an extension of the stream interface for reading from
a file (or any other stream in a Unix system).

.. word:: init.mind    ( -- addr ) |K|, "init-dot-mind"

   File stream that refers to a file that is read automatically at
   startup. The name of this file is :file:`init.mind`, and it
   contains all the essential definitions for a running forht system.

.. word:: line#		( -- addr ) |K|, "line-number"

      Address of the current line number in the current stream. The
      first line of a file has the number 1.

.. word:: do-stream |K|

      Execute the code in the current input stream.

.. word:: >infile	( 'textfile -- addr ) |K|, "to-infile"

   The TOS contains the address of a textfile structure: compute the
   address of its :word:`>infile` field. The field is one cell wide
   and contains the underlying C file pointer of type :c:type:`FILE*`
   for this stream.

.. word:: >infile-name	( 'textfile -- addr ) |K|, "to-infile-name"

   The TOS contains the address of a textfile structure: compute the
   address of its :word:`>infile-name` field. The field is one cell
   wide and contains a pointer to a null-terminated string that
   contains the name of the file for this stream. The field may also
   contain a null pointer if the file is not open or has no name.

.. word:: >current	( 'textfile -- addr ) |K|, "to-current"

   The TOS contains the address of a textfile structure: compute the
   address of its :word:`>current` field. This field is one cell wide
   and contains the last character read from the file or the "end of
   file" constant.

.. word:: >line#	( 'textfile -- addr ) |K|, "to-line-number"

   The TOS contains the address of a textfile structure: compute the
   address of its :word:`line#` field. The field is one cell wide and
   contains the current line number of this stream.

.. word:: >caller       ( 'textfile -- addr ) |K|, "to-caller"

   Return the address of the caller field of a textfile structure. The
   field is one cell wide and contains the address of a text file in
   which the current text file has been defined. If such a file does
   not exist, the value is 0.

.. word:: /textfile     ( -- n ) |K|, "per-textfile"

      	Number of bytes in a file stream structure.

.. word:: file-init     ( new-file caller -- )

   Initialise a new textfile structure. *new-file* is the address of a
   memory region of :word:`/textfile` bytes. *caller* is either the
   address of an existing textfile structure or 0. If it is not 0, it
   contains the address of the file in which the current file was
   defined.

   The word :word:`file-init` then generates a new textfile structure
   at *new-file*. Its :word:`>caller` field is set to *caller*.


Implementation
^^^^^^^^^^^^^^

.. word:: file-forward	( -- ) |K|

   Read one character from the current file stream and store it in its
   :word:`>current` field. :word:`line#` is updated if the character
   is an "end of line" symbol.

.. word:: file-current@	( -- char ) |K|, "file-current-fetch"

   Put the character at the current position of the current file
   stream onto the stack.

.. word:: file-eof	( -- flag ) |K|, "file-e-o-f"

   Test whether the end of the current file stream is reached.
