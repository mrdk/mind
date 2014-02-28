=======
Streams
=======

Streams are an abstraction for the input from a source that produces
data continuously. There is always one "current stream", with its
address in `'instream`.

.. word:: >get	        ( stream -- addr ) |K|, "to-get"

   The TOS contains the address of a stream structure: compute the
   address of its `get` routine. The routine has the signature
   :stack:`( -- )`.

.. word:: >i	        ( stream -- addr ) |K|, "to-i"

   The TOS contains the address of a stream structure: compute the
   address of its `i` routine. The routine has the signature :stack:`(
   -- char )`.

.. word:: >i?	        ( stream -- addr ) |K|, "to-i-question"

   The TOS contains the address of a stream structure: compute the
   address of its `i?` routine. The routine has the signature
   :stack:`( -- flag )`.

.. word:: /stream	( -- n ) |K|, "per-stream"

   Number of bytes in a stream data structure.

.. word:: 'instream	( -- addr ) |K|, "tick-instream"

   Variable that contains the address of the current stream.

.. word:: get	        ( -- ) |K|

   Read one element of the current stream. `line#` is updated if the
   character is an "end of line" symbol.

.. word:: i	        ( -- n ) |K|

   Put the value at the current position of the current stream onto
   the stack. This is only defined if `i?` returns `true`. A stream
   may however return a specific end-of-stream value.

.. word:: i?            ( -- flag ) |K|, "i-question"

   Return `true` if the end of the stream is not yet reached.


File Streams
------------

A file stream is an extension of the stream interface for reading from
a file (or any other stream in a Unix system).

.. word:: this-file    ( -- addr ) |K|

   Variable that contains the address of the file stream which is
   currently read.

.. word:: with-file    |K|

   Make the content of `this-file` the current stream (by
   storing it in `'instream`).

.. word:: init.mind    ( -- addr ) |K|, "init-dot-mind"

   File stream that refers to a file that is read automatically at
   startup. The name of this file is :file:`init.mind`, and it
   contains all the essential definitions for a running forht system.

.. word:: line#		( -- addr ) |K|, "line-number"

   Address of the current line number in the current file stream. The
   first line of a file has the number 1.

.. word:: do-stream |K|

   Execute the code in the current file stream.

.. word:: >infile	( tstream -- addr ) |K|, "to-infile"

   The TOS contains the address of a textfile structure: compute the
   address of its `>infile` field. The field is one cell wide
   and contains the underlying C file pointer of type :c:type:`FILE*`
   for this stream.

.. word:: >infile-name	( tstream -- addr ) |K|, "to-infile-name"

   The TOS contains the address of a textfile structure: compute the
   address of its `>infile-name` field. The field is one cell
   wide and contains a pointer to a null-terminated string that
   contains the name of the file for this stream. The field may also
   contain a null pointer if the file is not open or has no name.

.. word:: >current	( tstream -- addr ) |K|, "to-current"

   The TOS contains the address of a textfile structure: compute the
   address of its `>current` field. This field is one cell wide
   and contains either the latest character read from the file or
   `#eof`.

.. word:: >line#	( tstream -- addr ) |K|, "to-line-number"

   The TOS contains the address of a textfile structure: compute the
   address of its `line#` field. The field is one cell wide and
   contains the current line number of this stream.

.. word:: >caller       ( tstream -- addr ) |K|, "to-caller"

   Return the address of the caller field of a textfile structure. The
   field is one cell wide and contains the address of a text file in
   which the current text file has been defined. If such a file does
   not exist, the value is 0.

.. word:: /textfile     ( -- n ) |K|, "per-textfile"

   Number of bytes in a textfile structure.

.. word:: textfile0     ( -- tstream ) |K|, "textfile-0"

   Address of the prototype for the textfile structure. It has a size
   of `/textfile` bytes. The fields are already initialised,
   ready for a call to `file-open`.

.. word:: file-open     ( str tstream -- ) |K|

   Open a file for the use in a text stream. *tstream* must not be
   already opened. *str* is the name of the file, which is opened in
   reading mode.

   If the opening of the file was successful, `errno` is set to
   0 and the first byte of the file is read into `>current`. If
   the file is empty, the content of `>current` is `#eof`.
   Otherwise, the cause for the failure can be read from
   `errno`.

.. word:: file-close    ( tstream -- ) |K|

   Close a text stream. If an error occurs, it is stored in
   `errno`. Otherwise, `errno` contains 0.

.. word:: errno         ( -- addr ) |K|

   This word provides access to the libc variable *errno*. If an error
   occurs during the call of a library function, it is set to a value
   that provides information about the nature of that error, but it is
   usually left unchanged all went according to plan. Any error value
   for `errno` is different from 0. So it is possible to set
   `errno` to 0 before a word is executed and then use
   `errno` to check for an error.

   Some words do however set `errno` to 0 after correct
   execution: this is then remarked in the explanation of this word.


Implementation
^^^^^^^^^^^^^^

These are words that should not usually called directly, but only
through a file stream object.

.. word:: file-get	( -- ) |K|

   Read one character from the current file stream and store it in its
   `>current` field. `line#` is updated if the character is an "end of
   line" symbol.

   If the end of the file is reached, it is closed automatically.

.. word:: file-i	( -- char ) |K|, "file-i"

   Put the character at the current position of the current file
   stream onto the stack.

.. word:: file-i?	( -- flag ) |K|, "file-i-question"

   Test whether the end of the current file stream is not yet reached.


Line Streams
------------

This stream is used to read a text file line by line. `i` returns a
pointer to a string which contains the line that was read last --
including a final linefeed character. The string is allocated and
managed by the stream object. Pointers to characters in the current
line are valid until the next line is read.

.. word:: /lines        ( -- n ) |K|, "per-textfile"

   Number of bytes in a textfile structure.

.. word:: lines-open    ( str linestream -- ) |K|

   Open a file for the use in a line stream. *linestream* must not be
   already opened. *str* is the name of the file, which is opened in
   reading mode.

   If the opening of the file was successful and the file is not
   empty, the first line of the file is read. 

   The cause of a failure can be read from `errno`, which is set to 0
   in case of a success. (Opening an empty file causes no error.)

.. word:: lines-close   ( linestream -- ) |K|

   Close a line stream. If an error occurs, it is stored in `errno`.
   Otherwise, `errno` contains 0.

.. word:: lines-get	( -- ) |K|

   Try to read one line from the current stream. If a line is has been
   read successfully, `line#` is incremented. When the end of the file
   is reached, it is closed automatically.

.. word:: lines-i	( -- char ) |K|, "lines-i"

   Return the pointer to the beginning of the current line.

.. word:: lines-i?	( -- flag ) |K|, "lines-i-question"

   Test whether the end of the current stream is not yet reached.



Low Level I/O
-------------

.. word:: stdin         ( -- file ) "standard-in"
          stdout        ( -- file ) "standard-out"
          stderr        ( -- file ) "standard-err"

   The standard Unix character streams, for input, output and error
   output. `stdin` can be used as the `>infile` field of a line stream.
