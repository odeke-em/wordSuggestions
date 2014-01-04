Word Suggestions:
==============================================================================
  A spell checker that generates for the user suggestions of words from a 

  dictionary, based off their similarity to the query.

Goal: 
=====
  Provide word suggestions based off context and not just spelling. 

  Enable the user to be able to add words they think should be 

  recognized next time, to the dictionary.

Usage:
========
  To fire up the program which now has a GUI:

  + make && ./exec/main resources/wordlist.txt resources/stitched.ch 0.75

    Expands to:

      To run the autoCorrect program with:

	+  resources/wordlist.txt as the dictionary/source of vocabulary.

	+  resources/stitched.ch as the file to correct.

	+  0.75 as the match threshold percentage.

  + make && ./exec/main

    Expands to:

      Run the autoCorrect program:

      + With the default dictionary, and default threshold match percentage.

      + Reading from standard input.

  NOTE: The threshold match percentage should be expressed as a number over 100
    ie 0.9 translates to 90%, 0.35 translates to 35%

Future features:
=================

  + Algorithm optimization, and use of memory mapping for shared 

    resources eg the dictionary.

  + Context detection instead of plain word lookup/matching from a dictionary 

    and word edit calculations.

  + Building of vocabulary from learnt words.

  + Conjugation detection. 

Hierachy:
==========

  + All the \*.c \*.h files are in directory src/

  + All the \*.o and executable files will be stored in directory exec/

  + All sample files will be stored in directory samples/

  + All resources like the dictionary will be stored in directory resources/

Samples:

  + Sample output file samples/warOWCorrected.txt and samples/warOWLearnt.txt 

    included as the result of passing in 'waroftheworlds.txt' as the 

    subject file.


Dependancies:
=============

  + GTK. 
  
  + C compiler [dev done with gcc version 4.6.3]
