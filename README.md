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
  First run:	

      make

    + For an interactive session:
  
       cd exec OR make spellCorrect && cd exec
  
       ./spellCorrect 

    + For testing out correction of a file:

       cd exec OR make autoCorrect && cd exec

      ./autoCorrect <sourcePath> <destinationOfLearntWords> > suggestions.txt

    +To test out the multi-threaded autoCorrection:

      cd exec OR make workSplitter && cd exec

      ./workSplitter <sourcePath> <nThreads_or_Cores_available>


Future features:
=================

  + Algorithm optimization, and use of memory mapping for shared 

    resources eg the dictionary.

  + Context detection instead of plain  word lookup/matching from a dictionary.

  + Conjugation detection. 

  + Hash-precomputation of dictionary words, and string interning.

Hierachy:
==========

  + All the \*.c files are in directory src/

  + All the \*.h files are in directory include/

  + All the \*.o and executable files will be stored in directory exec/

  + All sample files from will be stored in directory samples/

  + All resources like the dictionary will be stored in directory resources/

Samples:

  + Sample output file samples/warOWCorrected.txt and samples/warOWLearnt.txt 

    included as the result of passing in 'waroftheworlds.txt' as the 

    subject file.
