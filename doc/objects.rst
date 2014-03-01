=========
 Objects
=========

The object system of *mind* is based on the idea that at every moment
there is exactly one *active object*.

Objects belong to a class, and every instance of a class may also
store its own data. Therefore the full identity of the current object
is given by two words, `class` and `this`.

.. word:: class         ( -- addr ) |K|

   Return the address of the class data for the active object.

.. word:: !class        ( addr -- ) |K|, "store-class"

   Make *addr* the new class of the current object.

.. word:: this          ( -- addr ) |K|

   Return the address of the instance data for the active object.

.. word:: !this        ( addr -- ) |K|, "store-this"

   Make *addr* the new address of the instance data for the current
   object.
