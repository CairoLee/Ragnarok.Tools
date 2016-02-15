#include "mod_sound.h"

#include <string.h>

extern GtkWidget *window;

GtkWidget *lvSound;
GtkListStore *storeSound;
GtkWidget *btnMSUp;
GtkWidget *btnMSDown;
GtkWidget *btnMSAdd;
GtkWidget *btnMSDel;

LIBGLADE_SIGNAL_FUNC void OpenDialogModSound(GtkMenuItem *menuitem, gpointer user_data)
{
  if (actor->act == NULL)  return;

  GtkWidget *dialog;
  dialog = gtk_dialog_new_with_buttons(_("Add/Delete sound"), GTK_WINDOW(window),
             GTK_DIALOG_DESTROY_WITH_PARENT, NULL);
  gtk_dialog_add_button(GTK_DIALOG(dialog), "gtk-close", GTK_RESPONSE_CLOSE);

  MSDBuildDialog(dialog);
  g_signal_connect_swapped(GTK_OBJECT(dialog), "response", G_CALLBACK(MSDResponse), dialog);
  MSDSetInitial();
  gtk_widget_show_all(dialog);
}

void MSDProcUpSel(GtkButton *button, gpointer user_data)
{
  int no = MSDGetSelectedRow();
  if (no < 0)  return;

  if (no == 0)  return;
  actor->SwapSound(no, no-1);
  MSDSetInitial();
  MSDSetSelection(no-1);
}

void MSDProcDownSel(GtkButton *button, gpointer user_data)
{
  int no = MSDGetSelectedRow();
  if (no < 0)  return;
                                                                                
  if (no == actor->GetNumSounds()-1)  return;
  actor->SwapSound(no, no+1);
  MSDSetInitial();
  MSDSetSelection(no+1);
}

void MSDProcAddLast(GtkButton *button, gpointer user_data)
{
  char fname[FILENAME_MAX];
  if (GetFileName(fname, sizeof(fname), _("Select file that is added"), true, FILTER_WAV | FILTER_NONE) == false)  return;

  int len = (int)strlen(fname);
  int i;
  for (i=len; i>=0; i--) {
    if (fname[i] == '/')  break;
  }
  if (i != 0)  i++;

  actor->AddSound(&fname[i]);
  MSDSetInitial();
}

void MSDProcDelSel(GtkButton *button, gpointer user_data)
{
  int no = MSDGetSelectedRow();
  if (no < 0)  return;

  actor->DelSound(no);
  
  MSDSetInitial();
}

int MSDGetSelectedRow(void)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(lvSound));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(lvSound));
  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return -1;
  int i=0;
  do {
    if (gtk_tree_selection_iter_is_selected(selection, &iter)) {
      return i;
    }
    i++;
  } while  (gtk_tree_model_iter_next(model, &iter));

  return -1;
}
 
void MSDSetSelection(int row)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(lvSound));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(lvSound));
  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return;
  for (int i=1; i<=row; i++) {
    if (gtk_tree_model_iter_next(model, &iter) == FALSE)  return;
  }
  gtk_tree_selection_select_iter(selection, &iter);
}
 
void MSDSetInitial(void)
{
  gtk_list_store_clear(storeSound);
  
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(lvSound));
  for (int i=0; i<actor->GetNumSounds(); i++) {
    gtk_list_store_append(storeSound, &iter);
    gtk_list_store_set(storeSound, &iter, 0, actor->GetSoundFN(i), -1);
  }
}

void MSDBuildDialog(GtkWidget *dialog)
{
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *vbox2;
  GtkWidget *dumlabel1;
  GtkWidget *dumlabel2;
  GtkWidget *dumlabel3;
  GtkWidget *hbox2;
  GtkWidget *scrwin;

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox1, FALSE, FALSE, 0);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

  scrwin = gtk_scrolled_window_new(NULL, NULL);
  gtk_box_pack_start(GTK_BOX(hbox1), scrwin, TRUE, TRUE, 0);

  storeSound = gtk_list_store_new(1, G_TYPE_STRING);
  lvSound = gtk_tree_view_new_with_model(GTK_TREE_MODEL(storeSound));
  gtk_widget_set_size_request(lvSound, 100, 150);
  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)0);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(lvSound), -1, "filename", renderer, "text", 0, NULL);
  gtk_container_add(GTK_CONTAINER(scrwin), lvSound);

  vbox2 = gtk_vbox_new(TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox1), vbox2, TRUE, FALSE, 0);

  dumlabel1 = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(vbox2), dumlabel1, FALSE, FALSE, 0);

  btnMSUp = gtk_button_new_with_label(_("Up"));
  gtk_box_pack_start(GTK_BOX(vbox2), btnMSUp, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(btnMSUp), "clicked", GTK_SIGNAL_FUNC(MSDProcUpSel), NULL);

  dumlabel2 = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(vbox2), dumlabel2, FALSE, FALSE, 0);

  btnMSDown = gtk_button_new_with_label(_("Down"));
  gtk_box_pack_start(GTK_BOX(vbox2), btnMSDown, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(btnMSDown), "clicked", GTK_SIGNAL_FUNC(MSDProcDownSel), NULL);

  dumlabel3 = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(vbox2), dumlabel3, FALSE, FALSE, 0);

  hbox2 = gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox2, FALSE, FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox2), 20);

  btnMSAdd = gtk_button_new_with_label(_("Add last"));
  gtk_box_pack_start(GTK_BOX(hbox2), btnMSAdd, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(btnMSAdd), "clicked", GTK_SIGNAL_FUNC(MSDProcAddLast), NULL);

  btnMSDel = gtk_button_new_with_label(_("Del. selected"));
  gtk_box_pack_start(GTK_BOX(hbox2), btnMSDel, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(btnMSDel), "clicked", GTK_SIGNAL_FUNC(MSDProcDelSel), NULL);
}

void MSDResponse(GtkDialog *dialog, gint arg1, gpointer user_data)
{
  gtk_widget_destroy(GTK_WIDGET(user_data));
}

