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

A *reference* is a pair of `this` and `class` values. The following
words allow to save them in memory and restore them.

.. word:: ref!         ( addr -- ) |K|, "ref-store"

   Copy a reference to the current object to the memory region
   starting at *addr*.

.. word:: ref@         ( addr -- ) |K|, "ref-fetch"

   Make the reference that is stored in the memory region at *addr*
   the active object.

.. word:: /ref         ( -- n ) |K|, "per-ref"

   Return the size of a reference in bytes.
