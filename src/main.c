// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <string.h>
#include <gtk/gtk.h>

#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "trie/Trie.h"
#include "hashlist/errors.h"
#include "hashlist/hashList.h"
#include "hashlist/loadWords.h"

#define INTERACTIVE
#define THRESHOLD_RANK 0.50
#define NO_SUGGESTIONS_NOTIFICATION "No suggestions"

#define checkLoading(handle, funcPtr, libKey) {\
  funcPtr = dlsym(handle, libKey);\
  if ((error = dlerror()) != NULL) {\
    fputs(error, stderr);\
    exit(-1);\
  }\
}

static HashList *dict = NULL;

// This dict will allow overriding of the value of keys with collisions
static HashList *recentlyUsedDict = NULL;

typedef struct {
  int size;
  void **block;
} WordsBlock;

typedef struct SearchParam_ {
  WordsBlock *(*blockGenerator)(const char *);
  GtkWidget *resultsList;
} SearchParam;

void destroy(GtkWidget *widget, gpointer data) {
  gtk_main_quit ();
}

void clearListView(GtkWidget *list, const gchar *initV) {
  GtkListStore *store;
  GtkTreeModel *model;
  GtkTreeIter iter;
  store = GTK_LIST_STORE(
    gtk_tree_view_get_model(GTK_TREE_VIEW(list))
  );
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE) {
    return;
  } else {
    gtk_list_store_clear(store);
  }
}

void addToListView(GtkWidget *list, const gchar *str) {
  // Function copied straight from www.gtkforums.com
  GtkListStore *store;
  GtkTreeIter iter;
  store = GTK_LIST_STORE(
    gtk_tree_view_get_model(GTK_TREE_VIEW(list))
  );

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0 , str, -1);
}

void initList(GtkWidget *list) {
  GtkCellRenderer  *renderer;
  GtkTreeViewColumn *column;
  GtkListStore *store;
  
  // Create and append the single column
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes (
    "Suggestions", renderer, "text", 0, NULL
  );
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

  // Create the model and add to tree view
  // One column only
  store = gtk_list_store_new(1, G_TYPE_STRING);
  gtk_tree_view_set_model(
    GTK_TREE_VIEW(list), GTK_TREE_MODEL(store)
  );

  // Freeing the reference to the store
  g_object_unref(store);
}

WordsBlock *destroyWordsBlock(WordsBlock *wb) {
  if (wb != NULL && wb->block != NULL) {
    if (wb->size > 0) {
      void **it = wb->block; 
      void **end = it + wb->size;
      while (it != end) {
	if (*it != NULL) {
	  free(*it);
	  *it = NULL;
	}

	++it;
      }
    }

    free(wb->block);
    free(wb);
  }

  return wb;
}

long int freeRecentlyUsedDict(HashList *rUDict) {
  long int freeCount = 0;
  if (rUDict != NULL) {
    if (rUDict->list != NULL) {
      // Knowing that each entry in the recently used dict
      // is a memoize wordBlock of which the wordBlock's content 
      // are mere pointers to content in the word dictionary
      Element **it = rUDict->list; 
      Element **end = it + rUDict->size;
      while (it != end) {
	if (*it != NULL) {
	  Element *trav = *it;
          while (trav != NULL) {
            if (trav->value != NULL) {
               WordsBlock *wb = (WordsBlock *)trav->value;
               wb->size = 0; // Merely fake it's size to zero so that the 
                             // read-only pointers to the content of the 
                             // main dictionary aren't touched
	       wb = destroyWordsBlock(wb);
               ++freeCount;
            }
            Element *nextSav = trav->next;
            free(trav);
            trav = nextSav;
          }
          *it = NULL;
	}
        ++it;
      }
      free(rUDict->list);
      rUDict->list = NULL;
    }

    free(rUDict);
    rUDict = NULL;
  }

  return freeCount;
}


void searchTerms(GtkWidget *widget, gpointer *arg) {
  if (arg != NULL) {
    SearchParam *params = (SearchParam *)arg;
    GtkEntry *searchEntry = (GtkEntry *)widget;
    GtkWidget *lV = params->resultsList;
    const gchar *searchTerm = gtk_entry_get_text((GtkEntry *)searchEntry);

    if (searchTerm != NULL && lV != NULL) {
      const gint len = strlen(searchTerm)/1;
    #ifdef DEBUG
      printf(
      "\033[35msearchTerm: %s len: %d\033[00m\n", searchTerm, len
      );
    #endif
     
      WordsBlock *(*blockGenerator)(const char *) =\
	   params->blockGenerator;
      if (len && blockGenerator) { 
	clearListView(lV, NULL);
	WordsBlock *genBlock = blockGenerator(searchTerm);
	if (genBlock != NULL && genBlock->block != NULL) {
	  int i;
	  for (i=0; i < genBlock->size; ++i) {
	    char *wIn = (char *)genBlock->block[i];
	    addToListView(lV, wIn);
	  }
	  /* 
	    Deliberately clear the size to avoid freeing any 
	    word suggestions that will explicitly be freed once
	    their source dictionary is freed
	    genBlock->size = 0; 
	    genBlock = destroyWordsBlock(genBlock);
	  */
	} else {
	  addToListView(lV, NO_SUGGESTIONS_NOTIFICATION);
	}
      }
    }
  }
}

void runMenu(int argc, char *argv[]) {
  // Phase 1: Load all the necessary data and functions
  //          for the logical operation of program
  void *handle = dlopen("./exec/libaCorrect.so.1", RTLD_LAZY);
  if (handle == NULL) {
    fputs(dlerror(), stderr);
    exit(-1);
  }

  Element *(*getNext)(Element *);
  long int (*destroyHashList)(HashList *hl);
  HashList * (*loadWordsInFile)(const char *); 
  Element *(*getCloseMatches)(const char *, HashList *, const double);
  hashValue (*pjwCharHash)(const char *srcW);
  void (*insertElem)(HashList *hl, void *data, const hashValue hashCode);
  HashList *(*initHashListWithSize)(HashList *hl, const int size);
  Element **(*get)(HashList *hl, hashValue hashCode);

  char *error;
  // Loading the functions
  checkLoading(handle, getNext, "getNext");
  checkLoading(handle, getCloseMatches, "getCloseMatches");
  checkLoading(handle, loadWordsInFile, "loadWordsInFile");

  checkLoading(handle, insertElem, "insertElem");
  checkLoading(handle, pjwCharHash, "pjwCharHash");
  checkLoading(handle, initHashListWithSize, "initHashListWithSize");
  checkLoading(handle, destroyHashList, "destroyHashList");
  checkLoading(handle, get, "get");

  const char *dictPath = "./resources/wordlist.txt";
  if (argc >= 2) {
    dictPath = argv[1];
  }
#ifdef INTERACTIVIE
  fprintf(stderr, "\033[93mDictionary path: %s\n", dictPath);
#endif

  dict = loadWordsInFile(dictPath);
  if (dict == NULL) {
    fprintf(stderr, "FilePath :: \033[32m%s\033[00m\n", dictPath);
    return;
  } else {
    // Arbitrarily assuming 1/10 of the suggestions in the dict will be used
    recentlyUsedDict = initHashListWithSize(recentlyUsedDict, dict->size / 10);
  }

  float thresholdMatch = THRESHOLD_RANK;
  if (argc >= 4) {
    if (sscanf(argv[3], "%f", &thresholdMatch) != 1) {
    #ifdef DEBUG
      raiseWarning("Couldn't parse the threshold rank");;
    #endif
    thresholdMatch = THRESHOLD_RANK;
    }
  }

  WordsBlock *wordsBlockGen(const char *w) {
  #ifdef DEBUG
    printf("w: %s\n", w);
  #endif
    if (w == NULL) {
      return NULL;
    } else {
      // Remember we shouldn't mutate the data returned
      // it's memory will be managed after freeing it's source dict
      hashValue hV = pjwCharHash(w);
      // First try the recently used entries -- assuming we are maintaining
      // the same threshold match percentage
      Element **ruSav = get(recentlyUsedDict, hV);

      if (ruSav != NULL && *ruSav != NULL) { // Memoized hit
	printf("Memoized hit for word: %s\n", w);
	return (*ruSav)->value;
      }

      // Miss detected
      Element *match = getCloseMatches(w, dict, thresholdMatch);

      // Finally if there is no suggestion
      if (match == NULL) return NULL;

      WordsBlock *bSav = (WordsBlock *)malloc(sizeof(WordsBlock));

      int index=0, bSz = 100;
      bSav->block = (void **)malloc(sizeof(void *) * bSz);

      while (match != NULL) {
	if (index >= bSz) {
	  bSz += 10;
	  bSav->block =\
            (void **)realloc(bSav->block, sizeof(void *) * bSz);
	}

	bSav->block[index] = match->value;
	match = getNext(match);
	++index;
      }

      bSav->block =\
         (void **)realloc(bSav->block, sizeof(void *) * index);

      bSav->size = index;

      // Let's now memoize this value
      insertElem(recentlyUsedDict, bSav, hV);
      return bSav;
    }
  }

  // Phase 2: Get that GUI in
  static GtkWidget *window;

  GtkWidget *table;
  GtkWidget *qButton;
  GtkWidget *mainBox;
  GtkWidget *searchEntry;
  GtkWidget *resultsListView;
  GtkWidget *scrolled_window;

  gtk_init(&argc, &argv);

  resultsListView = gtk_tree_view_new();
  mainBox = gtk_vbox_new(FALSE, 1);

  window = gtk_dialog_new();

  gtk_signal_connect(
    GTK_OBJECT(window), "destroy",  (GtkSignalFunc)destroy, NULL
  );

  gtk_window_set_title(GTK_WINDOW(window), "WordSuggestor");
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  gtk_widget_set_usize(window, 500, 500);

  // Creating the new scrolled window
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);

  gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);

  // policy either: GTK_POLICY_AUTOMATIC or GTK_POLICY_ALWAYS
  gtk_scrolled_window_set_policy(
    GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS
  );

  searchEntry = gtk_entry_new();
  gtk_box_pack_start(
    GTK_BOX(mainBox), searchEntry, TRUE, TRUE, 0
  );

  gtk_box_pack_start(
    GTK_BOX(mainBox), scrolled_window, TRUE, TRUE, 0
  );

  gtk_box_pack_start(
    GTK_BOX(GTK_DIALOG(window)->vbox), mainBox, TRUE, TRUE, 0
  );

  gtk_widget_show(scrolled_window);
  gtk_widget_show(mainBox);

  table = gtk_table_new(1, 1, FALSE);

  gtk_table_set_row_spacings(GTK_TABLE(table), 1);
  gtk_table_set_col_spacings(GTK_TABLE(table), 1);


  // Pack the table into the scrolled window
  gtk_scrolled_window_add_with_viewport(
    GTK_SCROLLED_WINDOW(scrolled_window), table
  );

  initList(resultsListView);

  gtk_widget_show(searchEntry);
  gtk_widget_show(table);

  gtk_table_attach_defaults(
    GTK_TABLE(table), resultsListView, 0, 1, 0, 1
  );

  gtk_widget_show(resultsListView);

  SearchParam paramSav;
  paramSav.blockGenerator = wordsBlockGen;
  paramSav.resultsList = resultsListView;

  // Monitor any changes with the searchEntry
  g_signal_connect(
    searchEntry,"changed", G_CALLBACK(searchTerms), &paramSav
  );

  qButton = gtk_toggle_button_new_with_label("Exit");
  GTK_WIDGET_SET_FLAGS(qButton, GTK_CAN_DEFAULT);
  gtk_box_pack_start(
    GTK_BOX(GTK_DIALOG(window)->action_area), qButton, TRUE, TRUE, 0
  );

  g_signal_connect(
    qButton, "clicked", G_CALLBACK(destroy), NULL
  );
  gtk_widget_show(qButton);
 
  gtk_widget_show(window); 
  gtk_main();

  // Clean up
  destroyHashList(dict);
  long int freeCount = freeRecentlyUsedDict(recentlyUsedDict);
  printf("RUFreeCount: %ld\n", freeCount);
}

int main(int argc, char *argv[]) {
  runMenu(argc, argv);
  return 0;
}
