Arithmetics
-----------

Numbers
^^^^^^^

.. word:: 0		( -- 0 )

      This and other numbers are defined as Forth words to shorten the
      compiled code and to make the bootstrapping of the language
      easier.

.. word:: 1		( -- 1 )

.. word:: -1		( -- -1 )

.. word:: 2		( -- 2 )

.. word:: 1+		( n -- n' ) "one-plus"

   Add 1 to the TOS.

   .. source:: [Forth83]_

.. word:: 1-		( n -- n' ) "one-minus"

   Subtract 1 from the TOS.

   .. source:: [Forth83]_

.. word:: 2*		( n -- n' ) "two-times"

   Multiplication with 2, as signed integer.

   .. source:: [Forth83]_

.. word:: 2/		( n -- n' ) "two-divide"

   Division by 2, as signed integer.

   .. source:: [Forth83]_

.. word:: -		( n1 n2 -- n3 ) "minus"

   Compute the difference *n1* - *n2*.

   .. source:: [Forth83]_

.. word:: +		( n1 n2 -- n3 ) "plus"

   Compute the sum of *n1* and *n2*.

   .. source:: [Forth83]_

.. word:: *		( n1 n2 -- n3 ) "times"

   Compute the product of *n1* and *n2*

   .. source:: [Forth83]_

.. word:: /		( n1 n2 -- n3 ) "divide"

   Compute the quotient *n1* / *n2* as integer.

   Currently this is C arithmetics, with rounding towards 0. (It may
   be changed later.)

   .. source:: [Forth83]_

.. word:: mod		( n1 n2 -- n3 )

   Compute *n1* modulo *n2*

.. word:: /mod		( n1 n2 -- quot rem ) "divide-mod"

   *quot* is *n1* / *n2* and *rem* is *n1* modulo *n2*.

   .. source:: [Forth83]_

.. word:: u*		( n1 n2 -- n3 )

   Product of *n1* and *n2* as unsigned integers.

.. word:: u/		( n1 n2 -- n3 )

   Quotient of n1 and n2 as unsigned integers.

.. word:: abs		( n -- u ) "absolute"

   Compute the absolute value of the TOS.

   .. source:: [Forth83]_


Logic and Comparisons
^^^^^^^^^^^^^^^^^^^^^

.. word:: false		( -- flag )

   Boolean flag for false.

.. word:: true		( -- flag )

      Boolean flag for true.

.. word:: or		( n1 n2 -- n3 )

   Bitwise "or" of *n1* and *n2*.

   .. source:: [Forth83]_

.. word:: and		( n1 n2 -- n3 )

   Bitwise "and" of *n1* and *n2*.

   .. source:: [Forth83]_

.. word:: xor		( n1 n2 -- n3 ) "x-or"

   Bitwise exclusive "or" of *n1* and *n2*.

   .. source:: [Forth83]_

.. word:: invert	( n1 -- n2 )

   Bitwise negation of the TOS.

.. word:: =		( n1 n2 -- flag ) "equals"

   Test whether *n1* and *n2* are equal.

   .. source:: [Forth83]_

.. word:: <>		( n1 n2 -- flag )

   Test whether *n1* and *n2* are different.

.. word:: 0=		( n -- flag ) "zero-equals"

   Test whether the TOS is equal to 0. (This also inverts boolean
   flags.)

   .. source:: [Forth83]_

.. word:: 0<		( n -- flag ) "zero-less"

   Test whether TOS < 0

   .. source:: [Forth83]_

.. word:: 0>		( n -- flag ) "zero-greater"

   Test whether TOS > 0

   .. source:: [Forth83]_

.. word:: <		( n1 n2 -- flag ) "less-than"

   Test whether *n1* < *n2*.

   .. source:: [Forth83]_

.. word:: <=		( n1 n2 -- flag )

   Test whether *n1* <= *n2*.

.. word:: >		( n1 n2 -- flag ) "greater-than"

   Test whether *n1* > *n2*.

   .. source:: [Forth83]_

.. word:: >=		( n1 n2 -- flag )

   Test whether *n1* >= *n2*.

.. word:: u<		( n1 n2 -- flag ) "u-less-than"

   Test whether *n1* < *n2* as unsigned integers.

   .. source:: [Forth83]_

.. word:: u<=		( n1 n2 -- flag )

   Test whether *n1* <= *n2* as unsigned integers.

.. word:: u>		( n1 n2 -- flag )

   Test whether *n1* > *n2* as unsigned integers.

.. word:: u>=		( n1 n2 -- flag )

   Test whether *n1* >= *n2* as unsigned integers.

.. word:: within		( n n0 n1 -- flag )

   True if *n0* <= *n* <= *n1*. The sequence of integers is here
   viewed as cyclic; the word works therefore with unsigned integers
   as well as with signed ones.
