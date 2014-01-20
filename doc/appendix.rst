.. default-domain:: rst

Appendix
********

Format of the Documentation Files
=================================

The documentation of *mind* is written in `Sphinx
<http://sphinx-doc.org>`_, with an extension for Forth.

The extension is defined in the file :file:`forthdoc.py`. It provides
a new domain, **forth**, with the following new directives and roles:

.. directive:: .. forth:word:: word [ ( stack ) ] [ flags ] \
               [ "pronunciation" ]

   Defines a new Forth word. *word* is the name of the word. *stack*
   is its stack diagram, which must be surrounded by parentheses.
   *flags* is a short list of flags, which are separated by commas,
   and *pronunciation* is a pronunciation hint, surrounded by quotes.
   It is given only if the pronunciation is not obvious.

.. role:: forth:word

   This role is used to refer to a word in plain text. It generates a
   link to the definition of the word if its exists.

   In this documentation, the default role is :role:`forth:word`. It
   is therefore enough to surround words in backquotes to refer to
   them as Forth words.

.. role:: stack

   This role is used to mark stack diagrams, as in :literal:`:stack:`( n1
   -- n2 )``. It displays them it italics.

In this documentation, **forth** is the default domain.
