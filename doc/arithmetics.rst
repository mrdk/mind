Numbers
-------

There are two ways a number on the stack can be interpreted: as a
signed and an unsigned integer. Signed numbers are interpreted in
two's complement notation. Positive signed numbers are binary
equivalent to unsigned numbers of the same value. Memory addresses,
when on the stack, are traditionally interpreted as unsigned integers.

Arithmetic
^^^^^^^^^^

.. word:: +		( n1 n2 -- n3 ) |K|, "plus"

   Compute the sum of *n1* and *n2*. This is the same in signed and in
   unsigned arithmetic.

   .. source:: [Forth83]_

.. word:: -		( n1 n2 -- n3 ) |K|, "minus"

   Compute the difference *n1* - *n2*. This is the same in signed and
   in unsigned arithmetic.

   .. source:: [Forth83]_

.. word:: *		( n1 n2 -- n3 ) |K|, "times"

   Compute the product of *n1* and *n2*. This is the same in signed
   and unsigned arithmetic.

   .. source:: [Forth83]_

.. word:: /		( n1 n2 -- n3 ) |K|, "divide"

   Compute the quotient *n1* / *n2* as signed integer.

   Currently this is C arithmetics, with rounding towards 0. (It may
   be changed later.)

   .. source:: [Forth83]_

.. word:: u/		( n1 n2 -- n3 ) |K|

   Quotient of n1 and n2 as unsigned integers.

.. word:: mod		( n1 n2 -- n3 ) |K|

   Compute *n1* modulo *n2* in signed arithmetic.

.. word:: /mod		( n1 n2 -- quot rem ) |K|, "divide-mod"

   *quot* is *n1* / *n2* and *rem* is *n1* modulo *n2*. The
   computation is done in signed arithmetic.

   .. source:: [Forth83]_

.. word:: abs		( n -- u ) |K|, "absolute"

   Compute the absolute value of the TOS. The result can be
   interpreted as a signed and as an unsigned number.

   .. source:: [Forth83]_

.. word:: negate        ( n1 -- n2 ) |K|

   Computer the negative if the TOS.

   .. source:: [Forth83]_

.. word::  max          ( n1 n2 -- n3 )
           min          ( n1 n2 -- n3 )

   Compute the maximum and minimum of *n1* and *n2* in signed
   arithmetic.

   .. source:: [Forth83]_

.. word:: -1		( -- -1 ) |K|
          0		( -- 0 )  |K|
          1		( -- 1 )  |K|
          2		( -- 2 )  |K|

   These numbers are defined as Forth words to shorten the compiled
   code and to make bootstrapping of the language easier.

.. word:: 1+		( n -- n' ) |K|, "one-plus"

   Add 1 to the TOS.

   .. source:: [Forth83]_

.. word:: 1-		( n -- n' ) |K|, "one-minus"

   Subtract 1 from the TOS.

   .. source:: [Forth83]_

.. word:: 2*		( n -- n' ) |K|, "two-times"

   Multiplication with 2, as signed or unsigned integer. This is also
   a bitwise shift to the left.

   .. source:: [Forth83]_

.. word:: 2/		( n -- n' ) |K|, "two-divide"

   Division by 2, as signed integer. This is also a bitwise shift to
   the right.

   .. source:: [Forth83]_


Logic and Comparisons
^^^^^^^^^^^^^^^^^^^^^

.. word:: false		( -- flag ) |K|

   Boolean flag for false.

.. word:: true		( -- flag ) |K|

      Boolean flag for true.

.. word:: or		( n1 n2 -- n3 ) |K|

   Bitwise "or" of *n1* and *n2*.

   .. source:: [Forth83]_

.. word:: and		( n1 n2 -- n3 ) |K|

   Bitwise "and" of *n1* and *n2*.

   .. source:: [Forth83]_

.. word:: xor		( n1 n2 -- n3 ) |K|, "x-or"

   Bitwise exclusive "or" of *n1* and *n2*.

   .. source:: [Forth83]_

.. word:: invert	( n1 -- n2 ) |K|

   Bitwise negation of the TOS.

.. word:: =		( n1 n2 -- flag ) |K|, "equals"

   Test whether *n1* and *n2* are equal.

   .. source:: [Forth83]_

.. word:: <>		( n1 n2 -- flag ) |K|

   Test whether *n1* and *n2* are different.

.. word:: 0=		( n -- flag ) |K|, "zero-equals"

   Test whether the TOS is equal to 0. (This also inverts boolean
   flags.)

   .. source:: [Forth83]_

.. word:: 0<		( n -- flag ) |K|, "zero-less"

   Test whether TOS < 0

   .. source:: [Forth83]_

.. word:: 0>		( n -- flag ) |K|, "zero-greater"

   Test whether TOS > 0

   .. source:: [Forth83]_

.. word:: <		( n1 n2 -- flag ) |K|, "less-than"

   Test whether *n1* < *n2*.

   .. source:: [Forth83]_

.. word:: <=		( n1 n2 -- flag ) |K|

   Test whether *n1* <= *n2*.

.. word:: >		( n1 n2 -- flag ) |K|, "greater-than"

   Test whether *n1* > *n2*.

   .. source:: [Forth83]_

.. word:: >=		( n1 n2 -- flag ) |K|

   Test whether *n1* >= *n2*.

.. word:: u<		( n1 n2 -- flag ) |K|, "u-less-than"

   Test whether *n1* < *n2* as unsigned integers.

   .. source:: [Forth83]_

.. word:: u<=		( n1 n2 -- flag ) |K|

   Test whether *n1* <= *n2* as unsigned integers.

.. word:: u>		( n1 n2 -- flag ) |K|

   Test whether *n1* > *n2* as unsigned integers.

.. word:: u>=		( n1 n2 -- flag ) |K|

   Test whether *n1* >= *n2* as unsigned integers.

.. word:: within		( n n0 n1 -- flag ) |K|

   True if *n0* <= *n* <= *n1*. The sequence of integers is here
   viewed as cyclic; the word works therefore with unsigned integers
   as well as with signed ones.
