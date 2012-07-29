Stack Operations
----------------

Parameter Stack
^^^^^^^^^^^^^^^

.. word:: drop		( a -- )

   .. source:: [Forth83]_

.. word:: nip		( a b -- b )

.. word:: 2drop		( a b -- ) "two-drop"

   .. source:: [Forth83]_

.. word:: ?dup		( 0 -- 0 | n -- n n ) "question-dupe"

      Duplicate the TOS only if it is nonzero

.. word:: dup		( a -- a a ) "dupe"

   .. source:: [Forth83]_

.. word:: over		( a b -- a b a )

   .. source:: [Forth83]_

.. word:: under		( a b -- b a b )

.. word:: swap		( a b -- b a )

   .. source:: [Forth83]_

.. word:: rot		( a b c -- b c a ) "rote"

   .. source:: [Forth83]_

.. word:: -rot		( a b c -- c a b )

.. word:: sp@		( -- addr ) "s-p-fetch"

   Get the value of the stack pointer. ``sp@ @`` is equivalent to
   :word:`dup`.

   .. source:: [Forth83]_

.. word:: sp!		( addr -- )

      Make *addr* the new value of the stack pointer.

.. word:: s0		( -- addr ) "s-zero"

   Variable for the position of the stack pointer when the stack is
   empty

   .. source:: [Forth83]_


Return Stack
^^^^^^^^^^^^

.. word:: rdrop

      Remove the top value of the return stack.

.. word:: >r		( n -- ) "to-r"

   Move the TOS to the top of the return stack.

   .. source:: [Forth83]_

.. word:: r>		( -- n ) "r-from"

   Move the top of the return stack to the TOS.

   .. source:: [Forth83]_

.. word:: >rr		( n -- )

   Move the TOS to the second position of the return stack.

   .. source:: [Reva]_

.. word:: rr>		( -- n )

   Move the second entry of the return stack to the TOS.

   .. source:: [Reva]_

.. word:: r@		( -- n ) "r-fetch"

   Copy the top of the return stack to the TOS.

   .. source:: [Forth83]_

.. word:: r0		( -- addr )

      Variable for the position of the return stack pointer when the
      stack is empty
