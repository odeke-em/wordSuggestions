#Goal: Creating a spell checker and word's suggestor that learns and offers the
#user a chance to add in whichever words they want to a dictionary: 'wordlist.txt'

#To test current setup: 
#First run:	gcc -c wordTransition.c -o wordTransition.o
# For an interactive session:
#       gcc spellCorrect.c wordTransition.o -o spellCorrect -lpthread && ./spellCorrect
#
#For testing out with a file:
#Run: make OR
#       gcc -DALLPRINTWORDS wSearch.c wordTransition.o -o autoCorrect -lpthread && ./autoCorrect
