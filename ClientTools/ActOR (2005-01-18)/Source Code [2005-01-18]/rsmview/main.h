#ifndef __MY_MAIN__
#define __MY_MAIN__

#include <libintl.h>

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include "../common/global.h"
#include "rsmview.h"
#include "../common/pak.h"
#include "../common/vec.h"

void ModelSelect(GtkTreeView *tview, gpointer data);
bool FindTreeIter(GtkTreeIter *iter, GtkTreeModel **mod);
void MakeOrigFilename(char *fname, GtkTreeIter iter, GtkTreeModel *mod);

gint CloseMainWindow(GtkWidget *widget, GdkEvent *event, gpointer data);
gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data);
gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer data);


void SetTextDomain(char *progname);
void SignalConnect(void);
void BuildWindow(void);
void BuildMenu(GtkWidget *menubar);
void BuildTree(void);
void CreateTreeItem(GtkTreeIter *parent, char *filename);
void CreateTreeSubtree(GtkTreeIter *parent, char *subname, char *filename);
void AddTreeItem(GtkTreeIter *parent, char *filename);

void SelectColor(GtkMenuItem *menuitem, gpointer user_data);
void ViewReset(GtkMenuItem *menuitem, gpointer user_data);
void ChangeDispMode(GtkMenuItem *menuitem, gpointer user_data);


                                                                                
void SetTextDomain(char *progname);

#endif //  __MY_MAIN__

