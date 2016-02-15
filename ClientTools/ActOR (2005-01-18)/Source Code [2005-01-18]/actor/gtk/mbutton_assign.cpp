#include "mbutton_assign.h"

#include <stdlib.h>

GtkWidget *btnRMove;
GtkWidget *btnRRot;
GtkWidget *btnRZoom;
GtkWidget *btnMMove;
GtkWidget *btnMRot;
GtkWidget *btnMZoom;
GtkWidget *btnLMove;
GtkWidget *btnLRot;
GtkWidget *btnLZoom;
bool MBAfirstCheck = TRUE;

LIBGLADE_SIGNAL_FUNC void OpenDialogMouseButtonAssign(GtkMenuItem *menuitem, gpointer user_data)
{
  GtkWidget *dialog;
  dialog = gtk_dialog_new_with_buttons(_("Reassign mouse button"), GTK_WINDOW(window),
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_STOCK_CLOSE, GTK_RESPONSE_NONE,
                                       NULL);
  MBABuildDialog(dialog);

  g_signal_connect_swapped(GTK_OBJECT(dialog), "response", G_CALLBACK(gtk_widget_destroy), GTK_OBJECT(dialog));
  gtk_widget_show_all(dialog);
}

void MBABuildDialog(GtkWidget *dia)
{
  GtkWidget *frameL;
  frameL = gtk_frame_new(NULL);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dia)->vbox), frameL, TRUE, TRUE, 0);

  GtkWidget *hboxL;
  hboxL = gtk_hbox_new(TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frameL), hboxL);

  GSList *btnTYPE_L = NULL;
  btnLMove = gtk_radio_button_new_with_label(NULL, _("Move"));
  gtk_box_pack_start(GTK_BOX(hboxL), btnLMove, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnLMove), btnTYPE_L);
  btnTYPE_L = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnLMove));
  btnLRot = gtk_radio_button_new_with_label(NULL, _("Rot"));
  gtk_box_pack_start(GTK_BOX(hboxL), btnLRot, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnLRot), btnTYPE_L);
  btnTYPE_L = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnLRot));
  btnLZoom = gtk_radio_button_new_with_label(NULL, _("Zoom"));
  gtk_box_pack_start(GTK_BOX(hboxL), btnLZoom, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnLZoom), btnTYPE_L);
  btnTYPE_L = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnLZoom));

  GtkWidget *labelL;
  labelL = gtk_label_new(_("L button"));
  gtk_frame_set_label_widget(GTK_FRAME(frameL), labelL);
  gtk_label_set_justify(GTK_LABEL(labelL), GTK_JUSTIFY_LEFT);

  GtkWidget *frameM;
  frameM = gtk_frame_new(NULL);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dia)->vbox), frameM, TRUE, TRUE, 0);

  GtkWidget *hboxM;
  hboxM = gtk_hbox_new(TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frameM), hboxM);

  GSList *btnTYPE_M = NULL;
  btnMMove = gtk_radio_button_new_with_label(NULL, _("Move"));
  gtk_box_pack_start(GTK_BOX(hboxM), btnMMove, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnMMove), btnTYPE_M);
  btnTYPE_M = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnMMove));
  btnMRot = gtk_radio_button_new_with_label(NULL, _("Rot"));
  gtk_box_pack_start(GTK_BOX(hboxM), btnMRot, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnMRot), btnTYPE_M);
  btnTYPE_M = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnMRot));
  btnMZoom = gtk_radio_button_new_with_label(NULL, _("Zoom"));
  gtk_box_pack_start(GTK_BOX(hboxM), btnMZoom, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnMZoom), btnTYPE_M);
  btnTYPE_M = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnMZoom));

  GtkWidget *labelM;
  labelM = gtk_label_new(_("M button"));
  gtk_frame_set_label_widget(GTK_FRAME(frameM), labelM);
  gtk_label_set_justify(GTK_LABEL(labelM), GTK_JUSTIFY_LEFT);

  GtkWidget *frameR;
  frameR = gtk_frame_new(NULL);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dia)->vbox), frameR, TRUE, TRUE, 0);

  GtkWidget *hboxR;
  hboxR = gtk_hbox_new(TRUE, 0);
  gtk_container_add(GTK_CONTAINER(frameR), hboxR);

  GSList *btnTYPE_R = NULL;
  btnRMove = gtk_radio_button_new_with_label(NULL, _("Move"));
  gtk_box_pack_start(GTK_BOX(hboxR), btnRMove, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnRMove), btnTYPE_R);
  btnTYPE_R = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnRMove));
  btnRRot = gtk_radio_button_new_with_label(NULL, _("Rot"));
  gtk_box_pack_start(GTK_BOX(hboxR), btnRRot, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnRRot), btnTYPE_R);
  btnTYPE_R = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnRRot));
  btnRZoom = gtk_radio_button_new_with_label(NULL, _("Zoom"));
  gtk_box_pack_start(GTK_BOX(hboxR), btnRZoom, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(btnRZoom), btnTYPE_R);
  btnTYPE_R = gtk_radio_button_get_group(GTK_RADIO_BUTTON(btnRZoom));

  GtkWidget *labelR;
  labelR = gtk_label_new(_("R button"));
  gtk_frame_set_label_widget(GTK_FRAME(frameR), labelR);
  gtk_label_set_justify(GTK_LABEL(labelR), GTK_JUSTIFY_LEFT);

  MBASetInitDialog(MK_LBUTTON, btnLMove, btnLRot, btnLZoom);
  MBASetInitDialog(MK_MBUTTON, btnMMove, btnMRot, btnMZoom);
  MBASetInitDialog(MK_RBUTTON, btnRMove, btnRRot, btnRZoom);

  gtk_signal_connect(GTK_OBJECT(btnLMove), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)0);
  gtk_signal_connect(GTK_OBJECT(btnLRot), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)1);
  gtk_signal_connect(GTK_OBJECT(btnLZoom), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)2);
  gtk_signal_connect(GTK_OBJECT(btnMMove), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)3);
  gtk_signal_connect(GTK_OBJECT(btnMRot), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)4);
  gtk_signal_connect(GTK_OBJECT(btnMZoom), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)5);
  gtk_signal_connect(GTK_OBJECT(btnRMove), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)6);
  gtk_signal_connect(GTK_OBJECT(btnRRot), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)7);
  gtk_signal_connect(GTK_OBJECT(btnRZoom), "clicked", GTK_SIGNAL_FUNC(MBAProcRadio), (gpointer)8);
}

void MBASetInitDialog(int LMR, GtkWidget *move, GtkWidget *rot, GtkWidget *zoom)
{
  if (mouseAssign.pan == LMR) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(move), TRUE);
  } else if (mouseAssign.rot == LMR) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rot), TRUE);
  } else if (mouseAssign.zoom == LMR) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(zoom), TRUE);
  }
}

void MBAProcRadio(GtkWidget *button, gpointer user_data)
{
  if (MBAfirstCheck == FALSE)  return;
  MBAfirstCheck = FALSE;

  GtkWidget *ar[9];
  ar[0] = btnLMove; ar[1] = btnLRot; ar[2] = btnLZoom;
  ar[3] = btnMMove; ar[4] = btnMRot; ar[5] = btnMZoom;
  ar[6] = btnRMove; ar[7] = btnRRot; ar[8] = btnRZoom;

  int id1 = (int)user_data / 3;
  int id = (int)user_data;
  int ca[3][2] = {{1, 2}, {-1, 1}, {-2, -1}};
  int cb[3][2] = {{3, 6}, {-3, 3}, {-6, -3}};

  int i;
  // exclusive procedure
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ar[ id+cb[id1][0] ]))) {
    for (i=0; i<2; i++) {
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ar[ id+cb[id1][1]+ca[id%3][i] ])) == FALSE) {
        break;
      }
    }
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ar[ id+cb[id1][0]+ca[id%3][i] ]), TRUE);
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ar[ id+cb[id1][1] ]))) {
    for (i=0; i<2; i++) {
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ar[ id+cb[id1][0]+ca[id%3][i] ])) == FALSE) {
        break;
      }
    }
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ar[ id+cb[id1][1]+ca[id%3][i] ]), TRUE);
  }

  MBAfirstCheck = TRUE;
  MBASetState();
}

void MBASetState(void)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnLMove))) {
    mouseAssign.pan = MK_LBUTTON;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnLRot))) {
    mouseAssign.rot = MK_LBUTTON;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnLZoom))) {
    mouseAssign.zoom = MK_LBUTTON;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnMMove))) {
    mouseAssign.pan = MK_MBUTTON;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnMRot))) {
    mouseAssign.rot = MK_MBUTTON;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnMZoom))) {
    mouseAssign.zoom = MK_MBUTTON;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnRMove))) {
    mouseAssign.pan = MK_RBUTTON;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnRRot))) {
    mouseAssign.rot = MK_RBUTTON;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(btnRZoom))) {
    mouseAssign.zoom = MK_RBUTTON;
  }
}
