Control Flow
------------

Control Structures
^^^^^^^^^^^^^^^^^^

.. word:: ;;            |K|, |rt|, "semi-semi"

   Jump out of the current word.

.. word:: if;		( n -- ) |K|, |rt|, "if-semi"

   Jump out of the current word only if the TOS is nonzero.

.. word:: 0; 		( 0 -- | n -- n ) |K|, |rt|, "zero-semi"

   If the TOS is zero, drop it and jump out of the current word.

.. word:: execute	( xt -- ) |K|, |83|

   Execute the word with the given execution token.

.. word:: branch        |K|, |83|

   Unconditional jump. The cell following this word contains the
   address of the jump target.

.. word:: 0branch	( n -- ) |K|, "zero-branch"

      Conditional jump. If *n* is zero, jump to the address in the
      next cell. If *n* is nonzero, continue with the execution of the
      word after the next cell.


Error Handling
^^^^^^^^^^^^^^

.. word:: abort         |K|, |83|

   Stop the interpretation of the currently read text stream and
   return to the interactive mode by executing :word:`'abort`.

.. word:: 'abort	( -- addr ) |K|, "tick-abort"

   Variable that contains a word that does is called after an error
   has occurred; it is expected to reset the parameter stack and the
   return stack and then to start an interactive prompt.


Starting and Ending
^^^^^^^^^^^^^^^^^^^

.. word:: bye |K|

      Leave the Forth system immediately.

.. word:: boot

      Initialise the system completely and start the outer
      interpreter.


Command Line Parameters
^^^^^^^^^^^^^^^^^^^^^^^

The program :program:`mind` has the following command line parameters:

    .. option:: -e <cmd>

       Execute <cmd> and finish.
        
    .. option:: -x <cmd>

       Execute <cmd> and start interactive mode.

    .. option:: -h

       Print help text.

    The parameters set the following Forth variables:

.. word:: start-command	( -- addr ) |K|

      Variable containing the address of a string that is set by the
      command options :option:`-e` and :option:`-x`; otherwise its
      value is 0.

      If the value of :word:`start-command` is nonzero, then it
      contains a string that is executed after the file
      :file:`start.mind` is read and before the system switches to
      interactive mode (if it does).

.. word:: interactive-mode	( -- addr ) |K|

      Variable containing a flag that is set to :word:`false` by the
      command line options :option:`-e`. By default its value is
      :word:`true`.

      If the value of :word:`interactive-mode` is :word:`true`, then
      :program:`mind` switches to an interactive mode after startup.
