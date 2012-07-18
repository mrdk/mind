Glossary
========

.. glossary::

   cell 
        A unit of memory large enough to store an integer or an
        address.
  
   CFA
        Code Field Address. This is the address of a word such that
        it can be executed.

   flag
        A boolean value. If a word returns a flag, it is either 0, for
        "false", or -1, for true. If a word accepts a flag, every
        value different from 0 is interpreted as "true".

   TOS
        Top of Stack.

   word
        #. a Forth procedure,

	#. a sequence of non-space characters surrounded by spaces.

   variable
        A memory region to store the value of one cell (unless another
        size is specified). If a word is called a variable, it returns
        the address of this region.
