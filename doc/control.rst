Control Flow
============

Control Structures
------------------

.. word:: IF            ( n -- | Compile: -- addr cf ) |I|, |83|
          ELSE          ( Compile: addr1 cf1 -- addr2 cf2 ) |I|, |83|
          THEN          ( Compile: addr cf -- ) |I|, |83|

   Components of a control structure for conditional execution. They
   are used either in the form ``IF ... THEN`` or ``IF ... ELSE ...
   THEN``, otherwise an error is raised at compile time. If at the
   time when :word:`IF` is reached the TOS is zero, the code after
   :word:`IF` is execued, otherwise the code after :word:`ELSE`, if it
   exists. Afterwards the execution continues after :word:`THEN`.

   These words are the user interfaces to the more primitive words
   :word:`if,`, :word:`else,` and :word:`then,`.

.. word:: BEGIN       ( Compile: -- addr cf ) |I|, |83|
          WHILE       ( n -- | Compile: addr1 cf1 -- addr2 addr1 cf2 ) |I|, |83|
          REPEAT      ( Compile: addr1 .. addrn addr cf -- ) |I|, |83|

   Components of a control structure for loops. They are used in the
   form ::

       BEGIN ... WHILE ... WHILE ... REPEAT

   with an arbitrary number of :word:`WHILE`\s. The code between
   :word:`BEGIN` and :word:`REPEAT` is repeated until at some
   :word:`WHILE` the TOS is a nonzero number. It is possible to omit
   :word:`WHILE` completely; the result is an infinite loop.

   These words are the user interfaces to the more primitive words
   :word:`begin,`, :word:`while,` and :word:`repeat,`.

.. word:: ;;            |K|, |rt|, "semi-semi"

   Jump out of the current word.

.. word:: if;		( n -- ) |K|, |rt|, "if-semi"

   Jump out of the current word only if the TOS is nonzero.

.. word:: 0; 		( 0 -- | n -- n ) |K|, |rt|, "zero-semi"

   If the TOS is zero, drop it and jump out of the current word.

.. word:: execute	( xt -- ) |K|, |83|

   Execute the word with the given execution token.


Implementation of Control Structures
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. word:: branch        |K|, |83|

   Unconditional jump. The cell following this word contains the
   address of the jump target.

.. word:: 0branch	( n -- ) |K|, "zero-branch"

      Conditional jump. If *n* is zero, jump to the address in the
      next cell. If *n* is nonzero, continue with the execution of the
      word after the next cell.

.. word:: if,           ( n -- | Compile: -- addr )
          else,         ( Compile: addr1 -- addr2 )
          then,         ( Compile: addr -- )

   Building blocks for conditional execution. With them the structure
   ::

      IF ... ELSE ... THEN

   can be expressed as ::

      [ if, ] ... [ else, ] ... [ then, ]

   No check for correct nesting is done.

.. word:: begin,        ( Compile: -- addr )
          while,        ( n -- | Compile: addr1 -- addr2 addr1 )
          repeat,       ( Compile: addr -- )

   Building blocks for loops. With them the structure ::

       BEGIN ... WHILE ... WHILE ... REPEAT

   can be expressed as ::

       [ begin, ]  ... [ while, ] ... [ while, ] ... [ repeat, then, then, ]

   There must be as many :word:`then,` as there are :word:`while,`. No
   check for correct nesting is done.


Error Handling
--------------

.. word:: abort         |K|, |83|

   :word:`Defer` word that is called after an error has occured; it is
   it is expected to reset the parameter stack and the return stack
   and then to start an interactive prompt. Its default value in the
   kernel is :word:`bye`; in :file:`init.mind` it is then reset to
   :word:`command-interpret`.


Starting and Ending
-------------------

.. word:: bye |K|

      Leave the Forth system immediately.

.. word:: boot

      Initialise the system completely and start the outer
      interpreter.


Command Line Parameters
-----------------------

The program :program:`mind` has the following command line options:

.. option:: -e <cmd>

   Execute <cmd> and finish.

.. option:: -x <cmd>

   Execute <cmd> and start interactive mode.

.. option:: -h

   Print help text.

These options are called the *kernel options*, in contrast to the
options for the program interpreted by :program:`mind`. The options
behind the kernel options are called the *program options*.

The kernel options set the following Forth variables:

.. word:: start-command	( -- addr ) |K|

      Variable containing the address of a string that is set by the
      command options :option:`-e` and :option:`-x`; otherwise its
      value is 0.

      If the value of :word:`start-command` is nonzero, then it
      contains a string that is executed after the file
      :file:`init.mind` is read and before the system switches to
      interactive mode (if it does).

.. word:: interactive-mode	( -- addr ) |K|

      Variable containing a flag that is set to :word:`false` by the
      command line options :option:`-e`. By default its value is
      :word:`true`.

      If the value of :word:`interactive-mode` is :word:`true`, then
      :program:`mind` switches to an interactive mode after startup.

The complete command line parameters of :program:`mind` are accessible
through the following words:

.. word:: raw-argc         ( -- n ) |K|, "raw-arg-c"

   Return the number of command line parameters.

.. word:: raw-argv         ( -- addr ) |K|, "raw-arg-v"

   Address of an array of cells. The array has :word:`raw-argc` + 1
   elements, and the last element is always 0. The other elements are
   pointers to strings. These strings are the command line arguments
   of :word:`mind`. They are usually part of system memory and
   therefore immutable.

The program options are accessible through the following words:

.. word:: argc             ( -- n ) |K|, "arg-v"

   Return the number of program options.

.. word:: argv             ( -- addr ) |K|, "arg-c"

   Address of an array of cells. The array has :word:`argc` + 1
   elements, and the last element is always 0. The other elements are
   pointers to strings. These strings are the program options of
   :word:`mind`. They are usually part of system memory and therefore
   immutable.

   The array :word:`argv` is always the end part of :word:`raw-argv`.
