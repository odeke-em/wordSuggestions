#Goal: Creating a spell checker and word's suggestor that learns and will offer 
#the user a chance to add whichever words they want, to a resource. 
#For now they are added to 'learnt_words.txt'.

#To test current setup: 
#First run:	gcc -c wordTransition.c -o wordTransition.o
# For an interactive session:
#       gcc spellCorrect.c wordTransition.o -o spellCorrect -lpthread && ./spellCorrect
#
#For testing out with a file:
#Run: make OR
#       gcc -DALLPRINTWORDS wSearch.c wordTransition.o -o autoCorrect -lpthread && ./autoCorrect > suggestions.txt
#
#Sample output file 'suggestions.txt' included as the result of passing in 
#'Makefile' as the subject file.
