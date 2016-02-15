#include "frame_edit.h"

int numMaxFE;

GtkWidget *rbtnDEL;
GtkWidget *spinDEL;
GtkWidget *rbtnCOPY;
GtkWidget *spinCOPY;
GtkWidget *rbtnSWAP;
GtkWidget *spinSWAP1;
GtkWidget *spinSWAP2;

LIBGLADE_SIGNAL_FUNC void OpenDialogFrameEdit(GtkMenuItem *menuitem, gpointer user_data)
{
  GtkWidget *dialog;
  dialog = gtk_dialog_new_with_buttons(_("Frame edit"), GTK_WINDOW(window),
             GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, NULL);
  gtk_dialog_add_button(GTK_DIALOG(dialog), "gtk-close", GTK_RESPONSE_CLOSE);

  FEBuildDialog(dialog);
  g_signal_connect_swapped(GTK_OBJECT(dialog), "response", G_CALLBACK(FEResponse), dialog);
  FEDSetInitial();
  gtk_widget_show_all(dialog);
}

void FEDSetInitial(void)
{
  if (actor->IsReady()) {
    numMaxFE = actor->GetNumPattern(actor->curAct);
    FEDSetInitial_Spin();
  } else {
    numMaxFE = 1;
  }
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinSWAP2), numMaxFE);
}

void FEDSetInitial_Spin(void)
{
  GtkAdjustment *Adjust;

  Adjust = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spinDEL));
  Adjust->lower = 1;
  Adjust->upper = numMaxFE;
  Adjust->value = 1;
  gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(spinDEL), Adjust);

  Adjust = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spinCOPY));
  Adjust->lower = 1;
  Adjust->upper = numMaxFE;
  Adjust->value = 1;
  gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(spinCOPY), Adjust);

  Adjust = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spinSWAP1));
  Adjust->lower = 1;
  Adjust->upper = numMaxFE;
  Adjust->value = 1;
  gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(spinSWAP1), Adjust);

  Adjust = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spinSWAP2));
  Adjust->lower = 1;
  Adjust->upper = numMaxFE;
  Adjust->value = 1;
  gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(spinSWAP2), Adjust);
}

void FEDProcAny(void)
{
  if (actor->IsActReady() == false)  return;
  if (autoPlay) {
    OnBtnStop(NULL, NULL);
  }
  int idA, idB;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnDEL))) {
    if (numMaxFE == 1)  return;
    if (actor->curPat == numMaxFE-1)  actor->curPat--;
    idA = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinDEL));
    actor->DelPat(actor->curAct, idA-1);
    numMaxFE--;
    FEDSetInitial_Spin();
    FEDCheckMaxInEdit();
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnCOPY))) {
    idA = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinCOPY));
    actor->AddPat(actor->curAct, idA-1);
    for (int i=numMaxFE-1; i>=idA; i--) {
      actor->SwapPat(actor->curAct, i, i+1);
    }
    numMaxFE++;
    FEDSetInitial_Spin();
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnSWAP))) {
    idA = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinSWAP1));
    idB = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinSWAP2));
    actor->SwapPat(actor->curAct, idA-1, idB-1);
  }
  UndoRedoInit();
  ProcPatternChange(true);
}

void FEDCheckMaxInEdit(void)
{
  int n;
  n = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinDEL));
  if (n >= numMaxFE) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinDEL), numMaxFE);
  }
  n = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinCOPY));
  if (n >= numMaxFE) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinCOPY), numMaxFE);
  }
  n = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinSWAP1));
  if (n >= numMaxFE) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinSWAP1), numMaxFE);
  }
  n = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinSWAP2));
  if (n >= numMaxFE) {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinSWAP2), numMaxFE);
  }
}

void FEResponse(GtkDialog *dialog, gint arg1, gpointer user_data)
{
  if (arg1 == GTK_RESPONSE_APPLY) {
    FEDProcAny();
  } else {
    gtk_widget_destroy(GTK_WIDGET(user_data));
  }
}


void FEBuildDialog(GtkWidget *dialog)
{
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GSList *radiobutton1_group = NULL;
  GtkObject *spinbutton1_adj;
  GtkWidget *label1;
  GtkWidget *hbox2;
  GtkObject *spinbutton2_adj;
  GtkWidget *label2;
  GtkWidget *hbox3;
  GtkObject *spinbutton3_adj;
  GtkWidget *label3;
  GtkObject *spinbutton4_adj;
  GtkWidget *label4;

  vbox1 = gtk_vbox_new(FALSE, 10);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox1, FALSE, FALSE, 0);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

  rbtnDEL = gtk_radio_button_new_with_mnemonic (NULL, "");
  gtk_box_pack_start(GTK_BOX(hbox1), rbtnDEL, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnDEL), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnDEL));

  spinbutton1_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinDEL = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton1_adj), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox1), spinDEL, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("frame is erased."));
  gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label1), GTK_JUSTIFY_LEFT);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox2, TRUE, TRUE, 0);

  rbtnCOPY = gtk_radio_button_new_with_mnemonic (NULL, "");
  gtk_box_pack_start(GTK_BOX(hbox2), rbtnCOPY, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnCOPY), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnCOPY));

  spinbutton2_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
  spinCOPY = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton2_adj), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox2), spinCOPY, FALSE, FALSE, 0);

  label2 = gtk_label_new(_("frame is copied after this."));
  gtk_box_pack_start(GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_LEFT);

  hbox3 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox3, TRUE, TRUE, 0);

  rbtnSWAP = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_box_pack_start(GTK_BOX(hbox3), rbtnSWAP, FALSE, FALSE, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnSWAP), radiobutton1_group);
  radiobutton1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnSWAP));

  spinbutton3_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
  spinSWAP1 = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton3_adj), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox3), spinSWAP1, FALSE, FALSE, 0);

  label3 = gtk_label_new(_(","));
  gtk_box_pack_start(GTK_BOX(hbox3), label3, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label3), GTK_JUSTIFY_LEFT);

  spinbutton4_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
  spinSWAP2 = gtk_spin_button_new(GTK_ADJUSTMENT(spinbutton4_adj), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox3), spinSWAP2, FALSE, FALSE, 0);

  label4 = gtk_label_new(_("is swapped."));
  gtk_box_pack_start(GTK_BOX(hbox3), label4, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label4), GTK_JUSTIFY_LEFT);
}

