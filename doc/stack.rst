Stack Operations
----------------

Parameter Stack
^^^^^^^^^^^^^^^

.. word:: dup		( a -- a a ) |K|, |83|, "dupe"
          over		( a b -- a b a ) |K|, |83|
          under		( a b -- b a b ) |K|
          drop		( a -- ) |K|, |83|
          2drop		( a b -- ) |K|, |83|, "two-drop"
          nip		( a b -- b ) |K|
          swap		( a b -- b a ) |K|, |83|
          rot		( a b c -- b c a ) |K|, |83|, "rote"
          -rot		( a b c -- c a b ) |K|, "minus-rote"

   Basic stack operations.

.. word:: ?dup		( 0 -- 0 | n -- n n ) |K|, "question-dupe"

      Duplicate the TOS only if it is nonzero

.. word:: sp@		( -- addr ) |K|, |83|, "s-p-fetch"

   Get the value of the stack pointer. ``sp@ @`` is equivalent to
   :word:`dup`.

.. word:: sp!		( addr -- ) |K|, "s-p-store"

      Make *addr* the new value of the stack pointer.

.. word:: s0		( -- addr ) |K|, |83|, "s-zero"

   Variable for the position of the stack pointer when the stack is
   empty


Return Stack
^^^^^^^^^^^^

.. word:: rdrop         ( R: a -- )

      Remove the top value of the return stack.

.. word:: >r		( a -- ; R: -- a ) |K|, |83|, "to-r"

   Move the TOS to the top of the return stack.

.. word:: r>		( -- a; R: a -- ) |K|, |83|, "r-from"

   Move the top of the return stack to the TOS.

.. word:: >rr		( a -- ; R: b -- a b ) |K|, |rv|, "to-r-r"

   Move the TOS to the second position of the return stack.

.. word:: rr>		( -- a; R: a b -- b ) |K|, |rv|, "r-r-from"

   Move the second entry of the return stack to the TOS. The words
   :word:`rr>` and :word:`>rr` are helpful when one turns return stack
   manipulations into Forth words. As an example we may use the
   following sequence as part of a word ::

       r> + >r

   It adds a constant to the value on top of the return stack. To make
   this sequence a word of its own we must write a definition like ::

       : r+  ( n -- )   rr> + >rr ;

   because there is one more value on the return stack when :word:`r+`
   is called.

.. word:: r@		( -- a ) |K|, |83|, "r-fetch"

   Copy the top of the return stack to the TOS.

.. word:: rp@		( -- addr ) |K|, "r-p-fetch"

   Get the value of the return stack pointer. ``rp@ @`` is equivalent
   to :word:`r@`.

.. word:: rp!		( addr -- ) |K|, "r-p-store"

   Make *addr* the new value of the return stack pointer.

.. word:: r0		( -- addr ) |K|, "r-zero"

   Variable for the position of the return stack pointer when the
   return stack is empty.
