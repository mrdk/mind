Data structures
===============

Data Definitions
----------------

.. word:: Create        ( <word> -- ) |83|

   Create a new dictionary header. A statement :samp:`Create {foo}`
   creates a new header for a word :word:`foo`. When :word:`foo` is
   executed, it returns the address of the first cell behind its
   header. This address is also the value of :word:`here` after
   executing :word:`Create`.

   It is then possible to allocate space in the dictionary with
   :word:`allot`. A typical use of :word:`Create` is therefore in
   phrases like :samp:`Create {foo} ... allot`.

.. word:: Create,       ( 'interpreter <word> -- ) "create-comma"

   Create an header for the name *word* with the interpreter routine
   *'interpret*. This word performs the basic functionality of
   :word:`Create`.

.. word:: does>         |83|, "does"

   Used together with :word:`Create` or a word that calls it. The
   typical use is in a definition of the form :samp:`: {Foo} Create
   ... does> ... ;`.

.. word:: Variable      ( <word> -- ) |83|

   :samp:`Variable {xxx}` creates a word :word:`xxx` with signature
   ``( -- addr )``, where *addr* is the address of a newly allocated
   cell in the dictionary.

.. word:: Constant      ( n <word> -- ) |83|

   :samp:`n Constant {xxx}` creates a word :word:`xxx` with signature
   `( -- n )`.

.. word:: Alias         ( xt <word> -- )

   Define a new word `word` that executes the word with the execution
   token *xt*. It is typically used in the form :samp:`' {foo} Alias
   {bar}`, which defines *bar* as a new name for *foo*.

.. word:: Defer         ( <word> -- )

   Like :word:`Alias`, except that the new word has not yet a defined
   action. It must be later be set with :word:`is`. Before this has
   been done, the execution of the newly defined word causes an error.
   The word is typically used in the definition of mutually recursive
   words.

.. word:: is            ( xt <word> -- )

   Change the activity of a deferred word. *word* must be defined with
   :word:`Alias` or :word:`Defer`. The typical use is then :samp:`'
   {foo} is {bar}`, which makes *foo* the new activity of *bar*.


Dictionary Structure
--------------------

The internal structure of the dictionary.

.. word:: ^docol |K|

.. word:: ^dodefer |K|

.. word:: ^dovar |K|

.. word:: ^dodoes |K|

.. word:: >link         ( xt -- addr ) |K|, |83|, "to-link"

   Convert the execution token of a word to the address of its link
   field. This field is one cell wide and contains the address of the
   link field of the word defined before this -- or 0 of there is no
   previous word.

.. word:: link>         ( addr -- xt ) |K|, |83|, "from-link"

   Convert the address of the link field of a word to its execution
   token.

.. word:: >name         ( xt -- addr ) |K|, |83|, "to-name"

   Convert an execution token to the address of the name field of a
   word. The name field contains the address of the null-terminated
   string that is the name. This means that the command sequence ::

     ' word >name @

   leaves the address of the string `"word"` on the stack.

.. word:: >doer         ( xt -- addr ) |K|, "to-doer"

   Convert the execution token of a word to the address of its doer
   field. In words defined with :word:`does>`, it contains the address
   of the code that is eexcuted by them. Otherwise it is usually 0.

.. word:: >body         ( xt -- addr ) |K|, |83|, "to-body"

   Convert the execution token of a word to the address of its body.

.. word:: body>         ( addr -- xt ) |K|, |83|, "from-body"

   Convert the address of the body of a word to its execution token.

.. word:: flags@ |K|

.. word:: flags! |K|

.. word:: #immediate |K|
