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
   time when `IF` is reached the TOS is zero, the code after
   `IF` is execued, otherwise the code after `ELSE`, if it
   exists. Afterwards the execution continues after `THEN`.

   These words are the user interfaces to the more primitive words
   `if,`, `else,` and `then,`.

.. word:: BEGIN       ( Compile: -- addr cf ) |I|, |83|
          WHILE       ( n -- | Compile: addr1 cf1 -- addr2 addr1 cf2 ) |I|, |83|
          REPEAT      ( Compile: addr1 .. addrn addr cf -- ) |I|, |83|

   Components of a control structure for loops. They are used in the
   form ::

       BEGIN ... WHILE ... WHILE ... REPEAT

   with an arbitrary number of `WHILE`\s. The code between
   `BEGIN` and `REPEAT` is repeated until at some
   `WHILE` the TOS is a nonzero number. It is possible to omit
   `WHILE` completely; the result is an infinite loop.

   These words are the user interfaces to the more primitive words
   `begin,`, `while,` and `repeat,`.

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

   There must be as many `then,` as there are `while,`. No
   check for correct nesting is done.


Error Handling
--------------

.. word:: abort         |K|, |83|

   `Defer` word that is called after an error has occured; it is
   it is expected to reset the parameter stack and the return stack
   and then to start an interactive prompt. Its default value in the
   kernel is `bye`; in :file:`init.mind` it is then reset to
   `command-interpret`.


Starting and Finishing
----------------------

.. word:: bye |K|, |vf|

      Leave the Forth system immediately.

.. word:: boot |K|

   Deferred word. It is called after :file:`init.mind` has been read.
   Its purpose is to processes the command line options, print an
   initial message, and then to call `abort`.

   In the kernel it is originally initialised with `abort`.
   After :file:`init.mind` is executed, it is a call to
   `do-boot`.


Command Line Parameters
-----------------------

The program :program:`mind` can be called in the following way::

  mind [-h] [-e <cmd>] [-x <cmd>] [<file>] [...]

If *<file>* is present, it is opened and interpreted as Forth
code. Afterwards the command line options are interpreted. They are:

.. option:: -e <cmd>

   Execute *<cmd>* and finish.

.. option:: -x <cmd>

   Execute *<cmd>* and start interactive mode, unless there is a
   *<file>* argument.

.. option:: -h

   Print help text.

These options are called the *kernel options*, in contrast to the
options for the program interpreted by :program:`mind`. The options
behind the kernel options are called the *program options*.

The kernel options set the following Forth variables:

.. word:: arg-cmdline	( -- addr ) |K|

   Variable containing the address of a string that is set by the
   command options :option:`-e` and :option:`-x`; otherwise its value
   is 0.

   If the value of `arg-cmdline` is nonzero, then it contains a
   string that is executed after the file :file:`init.mind` is read
   and before the system switches to interactive mode (if it does).

.. word:: arg-interactive	( -- addr ) |K|

   Variable containing a flag that is set to `false` by the
   command line options :option:`-e`. By default its value is
   `true`.

   If the value of `arg-interactive` is `true`, then
   :program:`mind` switches to an interactive mode after startup.

The complete command line parameters of :program:`mind` are accessible
through the following words:

.. word:: raw-argc         ( -- n ) |K|, "raw-arg-c"

   Return the number of command line parameters.

.. word:: raw-argv         ( -- addr ) |K|, "raw-arg-v"

   Address of an array of cells. The array has `raw-argc` + 1
   elements, and the last element is always 0. The other elements are
   pointers to strings. These strings are the command line arguments
   of `mind`. They are usually part of system memory and
   therefore immutable.

The program options are accessible through the following words:

.. word:: argc             ( -- n ) |K|, "arg-v"

   Return the number of program options.

.. word:: argv             ( -- addr ) |K|, "arg-c"

   Address of an array of cells. The array has `argc` + 1
   elements, and the last element is always 0. The other elements are
   pointers to strings. These strings are the program options of
   `mind`. They are usually part of system memory and therefore
   immutable.

   The array `argv` is always the end part of `raw-argv`.
