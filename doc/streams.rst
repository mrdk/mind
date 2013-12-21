=======
Streams
=======

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
   and contains either the latest character read from the file or
   :word:`#eof`.

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

.. word:: file-open     ( str 'textfile -- ) |K|

   Open a file for the use in a text stream. *'textfile* must not be
   already opened. *str* is the name of the file, which is opened in
   reading mode.

   If the opening of the file was successful, :word:`errno` is set to
   0 and the first byte of the file is read into :word:`>current`. If
   the file is empty, the content of :word:`>current` is :word:`#eof`.
   Otherwise, the cause for the failure can be read from
   :word:`errno`.

.. word:: file-close    ( 'textfile -- ) |K|

   Close a text stream. If an error occurs, it is stored in
   :word:`errno`. Otherwise, :word:`errno` contains 0.

.. word:: errno         ( -- addr ) |K|

   This word provides access to the libc variable *errno*. If an error
   occurs during the call of a library function, it is set to a value
   that provides information about the nature of that error, but it is
   usually left unchanged all went according to plan. Any error value
   for :word:`errno` is different from 0. So it is possible to set
   :word:`errno` to 0 before a word is executed and then use
   :word:`errno` to check for an error.

   Some words do however set :word:`errno` to 0 after correct
   execution: this is then remarked in the explanation of this word.


Implementation
^^^^^^^^^^^^^^

These are words that should not usually called directly, but only
through a file stream object.

.. word:: file-forward	( -- ) |K|

   Read one character from the current file stream and store it in its
   :word:`>current` field. :word:`line#` is updated if the character
   is an "end of line" symbol.

   If the end of the file is reached, it is closed automatically.

.. word:: file-current@	( -- char ) |K|, "file-current-fetch"

   Put the character at the current position of the current file
   stream onto the stack.

.. word:: file-eof	( -- flag ) |K|, "file-e-o-f"

   Test whether the end of the current file stream is reached.
