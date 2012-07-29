Control Flow
------------

Control Structures
^^^^^^^^^^^^^^^^^^

.. word:: ;; |K|

      End the execution of the current word.

      .. source:: retroforth

.. word:: if;		( n -- ) |K|

      Leave the execution of the current word if the TOS is nonzero.

      .. source:: retroforth

.. word:: 0; 		( 0 -- | n -- n ) |K|

      If the TOS is zero, drop it and end the execution of the current
      word.

      .. source:: retroforth

.. word:: execute	( addr -- ) |K|

   Execute the word at addr.

   .. source:: [Forth83]_

.. word:: branch |K|

   Unconditional jump. The cell following this word contains the
   address of the jump target.

   .. source:: [Forth83]_

.. word:: 0branch	( n -- ) |K|

      Conditional jump. If *n* is zero, jump to the address in the
      next cell. If *n* is nonzero, continue with the execution of the
      word after the next cell.


Error Handling
^^^^^^^^^^^^^^

.. word:: abort |K|

   Stop the interpretation of the currently read text stream and
   return to the interactive mode by executing :word:`'abort`.

   .. source:: [Forth83]_

.. word:: 'abort	( -- addr ) |K|

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

:program:`mind` has the following command line parameters:

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
