// Author: Emmanuel Odeke <odeke@ualberta.ca>
#include <gtk/gtk.h>

#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "trie/Trie.h"
#include "hashmap/errors.h"
#include "hashmap/element.h"
#include "hashmap/radTrie.h"
#include "hashmap/radLoadWords.h"

#define THRESHOLD_RANK 0.60
#define NO_SUGGESTIONS_NOTIFICATION "No suggestions"

#define checkLoading(handle, funcPtr, libKey) {\
  funcPtr = dlsym(handle, libKey);\
  if ((error = dlerror()) != NULL) {\
    fputs(error, stderr);\
    exit(-1);\
  }\
}

static char *error = NULL;
static void *handle = NULL;

static RTrie *dict = NULL;
// This dict will allow overriding of the value of keys with collisions
static Trie *recentlyUsedTrie = NULL;

// Function pointers declared here
Trie *(*freshTrie)() = NULL;
Element *(*fetchNext)(Element *) = NULL;
RTrie * (*dictFromFile)(const char *) = NULL; 
int (*queryTrie)(Trie *tr, const char *, void **) = NULL;
Element *(*getMatches)(const char *, RTrie *, const double) = NULL;
Trie *(*addSeqWithLoad)(Trie *, const char *, void *, const TrieTag) = NULL;

typedef struct SearchParam_ {
  Element *(*suggestionsGen)(const char *);
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

void addToListView(GtkWidget *list, const gchar *str, const gdouble rank) {
  // Function copied straight from www.gtkforums.com
  GtkListStore *store;
  GtkTreeIter iter;
  store = GTK_LIST_STORE(
    gtk_tree_view_get_model(GTK_TREE_VIEW(list))
  );

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, str, 1, rank, -1);
}

void *freeMemoizedSuggestions(void *e) {
  if (e != NULL) {
    Element *sug = (Element *)e;
    Element *tmpNext;
    while (sug != NULL) {
      tmpNext = fetchNext(sug);
      // Not touching the data that explicitly belongs to the dict
      // Only the element shell itself
      free(sug);
      sug = tmpNext;
    }
  }

  return e;
}

void initList(GtkWidget *list) {
  GtkCellRenderer  *renderer;
  GtkTreeViewColumn *column, *rankColumn;
  GtkListStore *store;
  
  // Create and append the single column
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes (
    "Suggestions", renderer, "text", 0, NULL
  );
  rankColumn = gtk_tree_view_column_new_with_attributes (
    "Rank", renderer, "text", 1, NULL
  );
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), rankColumn);

  // Create the model and add to tree view
  // One column only
  store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_DOUBLE);
  gtk_tree_view_set_model(
    GTK_TREE_VIEW(list), GTK_TREE_MODEL(store)
  );

  // Freeing the reference to the store
  g_object_unref(store);
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

      // Freshen up the listView     
      clearListView(lV, NULL);

      Element *(*suggestionsGen)(const char *) = params->suggestionsGen;
      if (len && suggestionsGen) {
	Element *suggestions = suggestionsGen(searchTerm);
	if (suggestions != NULL) {
	  while (suggestions != NULL) {
	    addToListView(lV, suggestions->value, suggestions->rank);
	    suggestions = fetchNext(suggestions);
	  }
	} else {
	  addToListView(lV, NO_SUGGESTIONS_NOTIFICATION, 0);
	}
      }
    }
  }
}

void handleLibLoading() {
  // Phase 1: Load all the necessary data and functions
  //          for the logical operation of program
  checkLoading(handle, fetchNext, "getNext");
  checkLoading(handle, dictFromFile, "fileToRTrie");
  checkLoading(handle, getMatches, "getCloseMatches");

  // Trie functionality
  checkLoading(handle, freshTrie, "createTrie");
  checkLoading(handle, queryTrie, "searchTrie");
  checkLoading(handle, addSeqWithLoad, "addSequenceWithLoad");
}

void runMenu(int argc, char *argv[]) {
  handleLibLoading();

  const char *dictPath = "./resources/mergedWords.txt";
  if (argc >= 2) {
    dictPath = argv[1];
  }
#ifdef INTERACTIVIE
  fprintf(stderr, "\033[93mDictionary path: %s\n", dictPath);
#endif

  dict = dictFromFile(dictPath);
  if (dict == NULL) {
    fprintf(stderr, "FilePath :: \033[32m%s\033[00m\n", dictPath);
    return;
  }
  
  recentlyUsedTrie = freshTrie();

  float thresholdMatch = THRESHOLD_RANK;
  if (argc >= 3) {
    if (sscanf(argv[2], "%f", &thresholdMatch) != 1) {
    #ifdef DEBUG
      raiseWarning("Couldn't parse the threshold rank");;
    #endif
    thresholdMatch = THRESHOLD_RANK;
    }
  }

  printf("Threshold rank: %.3f\n", thresholdMatch);

  Element *wordsBlockGen(const char *w) {
  #ifdef DEBUG
    printf("w: %s\n", w);
  #endif
    if (w == NULL) {
      return NULL;
    } else {
      // Remember we shouldn't mutate the data returned
      // it's memory will be managed after freeing it's source dict
      // First try the recently used entries -- assuming we are maintaining
      // the same threshold match percentage
      void *ruSav = NULL;
      int found = queryTrie(recentlyUsedTrie, w, &ruSav);

      if (found == 1) { // Memoized hit
	printf("Memoized hit for word: %s\n", w);
	return ruSav;
      }

      // Miss detected
      Element *match = getMatches(w, dict, thresholdMatch);

      // Finally if there is suggestion, memoize it
      if (match != NULL) {
	recentlyUsedTrie = addSeqWithLoad(recentlyUsedTrie, w, match, HeapD);
      }

      return match;
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
  paramSav.suggestionsGen = wordsBlockGen;
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
}

void destroyRTrieWithMemLinearized(RTrie *dict) {
    if (dict != NULL) {
        RTrie * (*destroyRTrie)(RTrie *rt) = NULL;
        LinearizedTrie * (*destroyLinearizedTrie)(LinearizedTrie *l) = NULL;
        checkLoading(handle, destroyRTrie, "destroyRTrie");
        checkLoading(handle, destroyLinearizedTrie, "destroyLinearizedTrie");
        dict->meta = destroyLinearizedTrie((LinearizedTrie *)dict->meta);
        dict->meta = NULL;
        dict = destroyRTrie(dict);
    }
}

void cleanUpExit() {
  // Clean up
  destroyRTrieWithMemLinearized(dict);

  Trie *(*destroyTrieAndPayLoads)(Trie *t, void *(*loadFreer)(void *));
  checkLoading(handle, destroyTrieAndPayLoads, "destroyTrieAndPayLoads");

  recentlyUsedTrie =\
    destroyTrieAndPayLoads(recentlyUsedTrie, freeMemoizedSuggestions);

  fprintf(stderr, "\033[94m\nBye..\n\033[00m");
  exit(0);
}

void sigHandler(const int sig) {
  switch(sig) {
    case SIGINT:
      cleanUpExit();
      break;
    case SIGTERM:
      cleanUpExit();
      break;
    default:
      fprintf(stderr, "Unhandled signal: %d\n", sig);
  }
}

int main(int argc, char *argv[]) {
  // Load up the library first
  handle = dlopen("./exec/libaCorrect.so.1", RTLD_LAZY);

  if (handle == NULL) {
    fputs(dlerror(), stderr);
    exit(-1);
  }

  // Signal handling here
  static struct sigaction actH;
  actH.sa_handler = sigHandler;

  sigaction(SIGINT, &actH, NULL);
  sigaction(SIGTERM, &actH, NULL);
  sigaction(SIGSTOP, &actH, NULL);

  runMenu(argc, argv);

  // By this time it is safe to cleanUp and exit
  cleanUpExit();
  return 0;
}
