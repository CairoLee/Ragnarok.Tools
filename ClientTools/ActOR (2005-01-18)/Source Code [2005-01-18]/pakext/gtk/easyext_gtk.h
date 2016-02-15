#ifndef __MY_EASYEXT_GTK__
#define __MY_EASYEXT_GTK__

#include <libintl.h>
#include <locale.h>

#include <gtk/gtk.h>

#include "../main.h"
#include "../../common/pak.h"

extern int EasyExt(int argc, char **argv);
void EEExitDialog(void);

void EEClickBtnExtract(GtkButton *button, gpointer user_data);
bool EEFindTreeIter(GtkTreeIter *iter, GtkTreeModel **mod);
bool EEGetFilename(char *title, bool open, bool dir);

void EEExtractMain(GtkTreeIter iter, GtkTreeModel *mod);
void EEExtractChild(GtkTreeIter iter, GtkTreeModel *mod);
void EEExtractFile(GtkTreeIter iter, GtkTreeModel *mod);
void EEMakeOrigFilename(char *fname, GtkTreeIter iter, GtkTreeModel *mod);

#ifdef VIOLATION_CODE
void EEMakePatch(GtkButton *button, gpointer user_data);
#endif  // VIOLATION_CODE

bool EEExtractTemp(char *tmpfname, char *origfname);

gint EECloseDialog(GtkWidget *widget, GdkEvent *event, gpointer data);
void EECloseDialogBtn(GtkButton *button, gpointer data);
void EEAnyProc__(GtkButton *button, gpointer user_data);
void EESignalConnect(void);
void EEBuildWindow(void);
void EEBuildTree(void);

void EECreateItem(GtkTreeIter *parent, char *filename);
void EECreateSubtree(GtkTreeIter *parent, char *subname, char *filename);
void EEAddItem(GtkTreeIter *parent, char *filename);

#endif // __MY_EASYEXT_GTK__
