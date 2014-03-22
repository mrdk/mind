=========
 Objects
=========

The object system of *mind* is based on the idea that at every moment
there is exactly one *active object*.

Objects belong to a class, and every instance of a class may also
store its own data. Therefore the full identity of the current object
is given by two words, `class` and `this`. Together they form the
*active reference*.

.. word:: this          ( -- addr ) |K|

   Return the address of the instance data for the active object.

.. word:: this+         ( u -- addr ) |K|, "this-plus"

   Return the address *u* cells after `this`.

.. word:: @this         ( addr -- ) |K|, "at-this"

   Make *addr* the new address of the instance data for the current
   object.

.. word:: class         ( -- addr ) |K|

   Return the address of the class data for the active object.

.. word:: class+        ( u -- addr ) |K|, "class-plus"

   Return the address *u* cells after `class`.

.. word:: @class        ( addr -- ) |K|, "at-class"

   Make *addr* the new class of the current object.

Generally speaking, a *reference* is a pair of `this` and `class`
values. The following words allow to save them in memory and restore
them.

.. word:: ref!          ( addr -- ) |K|, "ref-store"

   Copy the active reference to the memory region starting at *addr*.

.. word:: @ref          ( addr -- ) |K|, "at-ref"

   Make the reference that is stored in the memory region at *addr*
   the active reference.

.. word:: /ref          ( -- n ) |K|, "per-ref"

   Return the size of a reference in bytes.


.. rubric:: The Object Stack

There is an *object stack* in which references to the active object
can be saved and later be restored. The active object is not part of
the object stack.

.. word:: op            ( -- addr ) |K|, "o-p"

   Variable containing the pointer to the top of the object stack.

.. word:: op0           ( -- addr ) |K|, "o-p-zero"

   Variable containing the pointer to the bottom of the object stack.
   To empty the object stack, the content of `op0` is stored in `op`.

.. word:: {             |K|, "scope"
          }             |K|, "end-scope"

   With these words, the reference to the active object can be saved
   and restored. After calling `{`, a copy of `class` and `this` is
   stored as a new reference in the object stack; the active object
   does however not change. `}` removes the reference at the top of the
   object stack and makes it the active object. Between `{` and `}`,
   the active reference can therefore be temporarily changed.
