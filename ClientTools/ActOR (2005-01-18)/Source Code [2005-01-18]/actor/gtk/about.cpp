#include <string.h>

#include "about.h"

LIBGLADE_SIGNAL_FUNC void OpenDialogAbout(GtkMenuItem *menuitem, gpointer user_data)
{
  GtkWidget *dialog;
  dialog = gtk_dialog_new_with_buttons(_("About..."), GTK_WINDOW(window),
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_STOCK_OK, GTK_RESPONSE_NONE,
                                       NULL);

  GtkWidget *label;
  char buf[1024];
  char buf2[1024];
  SNPRINTF(buf2, sizeof(buf2), _("You can edit ACT of Arcturus by this tool.\n\n"));
  SNPRINTF(buf, sizeof(buf), "%sBuild : %s %s", buf2, __DATE__, __TIME__);
  strcat(buf, "\n\nLicense: GNU GENERAL PUBLIC LICENSE version 2");
  label = gtk_label_new(buf);
  gtk_widget_set_size_request(label, -1, 100);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), label);
  gtk_widget_show_all(dialog);

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}
