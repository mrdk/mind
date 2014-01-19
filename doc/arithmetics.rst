Numbers
-------

There are two ways a number on the stack can be interpreted: as a
signed and an unsigned integer. Signed numbers are interpreted in
two's complement notation. Positive signed numbers are binary
equivalent to unsigned numbers of the same value. Memory addresses,
when on the stack, are traditionally interpreted as unsigned integers.

Arithmetics
^^^^^^^^^^^

.. word:: +		( n1 n2 -- n3 ) |K|, |83|, "plus"

   Compute the sum of *n1* and *n2*. This is the same in signed and in
   unsigned arithmetic.

.. word:: -		( n1 n2 -- n3 ) |K|, |83|, "minus"

   Compute the difference *n1* - *n2*. This is the same in signed and
   in unsigned arithmetic.

.. word:: *		( n1 n2 -- n3 ) |K|, |83|, "times"

   Compute the product of *n1* and *n2*. This is the same in signed
   and unsigned arithmetic.

.. word:: /		( n1 n2 -- n3 ) |K|, |83|, "divide"

   Compute the quotient *n1* / *n2* as signed integer. Currently this
   is C arithmetics, with rounding towards 0. (It may be changed
   later.)

.. word:: u/		( n1 n2 -- n3 ) |K|

   Quotient of n1 and n2 as unsigned integers.

.. word:: mod		( n1 n2 -- n3 ) |K|

   Compute *n1* modulo *n2* in signed arithmetic.

.. word:: /mod		( n1 n2 -- quot rem ) |K|, |83|, "divide-mod"

   *quot* is *n1* / *n2* and *rem* is *n1* modulo *n2*. The
   computation is done in signed arithmetic.

.. word:: abs		( n -- u ) |K|, |83|, "absolute"

   Compute the absolute value of the TOS. The result can be
   interpreted as a signed and as an unsigned number.

.. word:: negate        ( n1 -- n2 ) |K|, |83|

   Computer the negative if the TOS.

.. word::  max          ( n1 n2 -- n3 ) |83|
           min          ( n1 n2 -- n3 ) |83|

   Compute the maximum and minimum of *n1* and *n2* in signed
   arithmetic.

.. word:: -1		( -- -1 ) |K|
          0		( -- 0 )  |K|
          1		( -- 1 )  |K|
          2		( -- 2 )  |K|

   These numbers are defined as Forth words to shorten the compiled
   code and to make bootstrapping of the language easier.

.. word:: 1+		( n -- n' ) |K|, |83|, "one-plus"

   Add 1 to the TOS.

.. word:: 1-		( n -- n' ) |K|, |83|, "one-minus"

   Subtract 1 from the TOS.

.. word:: 2*		( n -- n' ) |K|, |83|, "two-times"

   Multiplication with 2, as signed or unsigned integer. This is also
   a bitwise shift to the left.

.. word:: 2/		( n -- n' ) |K|, |83|, "two-divide"

   Division by 2, as signed integer. This is also a bitwise shift to
   the right.


Logic and Comparisons
^^^^^^^^^^^^^^^^^^^^^

.. word:: true		( -- flag ) |K|, |83|
          false		( -- flag ) |K|, |83|

   Boolean flags for true and false. The value for :word:`true` is
   :word:`-1`, the value for :word:`false` is :word:`0`, so that
   either all bytes are set or all bytes are unset. This allows the
   replacement of control structures with arithmetic operations, as
   described in [ThinkingForth]_, p.243: It is then possible to
   replace code like ::

       IF  200  ELSE  0  THEN

   with ::

       200 and

.. word:: or		( n1 n2 -- n3 ) |K|, |83|

   Bitwise "or" of *n1* and *n2*.

.. word:: and		( n1 n2 -- n3 ) |K|, |83|

   Bitwise "and" of *n1* and *n2*.

.. word:: xor		( n1 n2 -- n3 ) |K|, |83|, "x-or"

   Bitwise exclusive "or" of *n1* and *n2*.

.. word:: not           ( n1 -- n2 ) |K|

   Bitwise negation of the TOS.

.. word:: =		( n1 n2 -- flag ) |K|, |83|, "equals"

   Test whether *n1* and *n2* are equal.

.. word:: <>		( n1 n2 -- flag ) |K|, "not-equals"

   Test whether *n1* and *n2* are different.

.. word:: 0=		( n -- flag ) |K|, |83|, "zero-equals"

   Test whether the TOS is equal to 0. (This also inverts boolean
   flags.)

.. word:: 0<>		( n -- flag ) |K|, "zero-not-equals"

   Test whether the TOS is different from 0. (This is used to convert
   an arbitrary number to a boolean flag.)

.. word:: 0<		( n -- flag ) |K|, |83|, "zero-less"

   Test whether TOS < 0

.. word:: 0>		( n -- flag ) |K|, |83|, "zero-greater"

   Test whether TOS > 0

.. word:: <		( n1 n2 -- flag ) |K|, |83|, "less-than"

   Test whether *n1* < *n2*.

.. word:: <=		( n1 n2 -- flag ) |K|, "less-than-or-equal"

   Test whether *n1* <= *n2*.

.. word:: >		( n1 n2 -- flag ) |K|, |83|, "greater-than"

   Test whether *n1* > *n2*.

.. word:: >=		( n1 n2 -- flag ) |K|, "greater-than-or-equal"

   Test whether *n1* >= *n2*.

.. word:: u<		( n1 n2 -- flag ) |K|, |83|, "u-less-than"

   Test whether *n1* < *n2* as unsigned integers.

.. word:: u<=		( n1 n2 -- flag ) |K|, "u-less-than-or-equal"

   Test whether *n1* <= *n2* as unsigned integers.

.. word:: u>		( n1 n2 -- flag ) |K|, "u-greater-than"

   Test whether *n1* > *n2* as unsigned integers.

.. word:: u>=		( n1 n2 -- flag ) |K|, "u-greater-than-or-equal"

   Test whether *n1* >= *n2* as unsigned integers.

.. word:: within	( n n0 n1 -- flag ) |K|, |vf|, |rt|

   True if *n0* <= *n* <= *n1*. The sequence of integers is here
   viewed as cyclic; the word works therefore with unsigned integers
   as well as with signed ones.
