#include "act_copy.h"

extern GtkWidget *window;

GtkWidget *rbtnCCW;
GtkWidget *rbtnCW;
GtkWidget *rbtnDETAIL;
GtkWidget *optAC11;
GtkWidget *optAC12;
GtkWidget *optAC2;
GtkWidget *optAC3;
GtkWidget* rbtnAC[2][8];

LIBGLADE_SIGNAL_FUNC void OpenDialogActCopy(GtkMenuItem *menuitem, gpointer user_data)
{
  GtkWidget *dialog;
  dialog = gtk_dialog_new_with_buttons(_("act copy"), GTK_WINDOW(window),
             GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, NULL);
  gtk_dialog_add_button(GTK_DIALOG(dialog), "gtk-close", GTK_RESPONSE_CLOSE);

  ACDBuildDialog(dialog);
  g_signal_connect_swapped(GTK_OBJECT(dialog), "response", G_CALLBACK(ACDResponse), dialog);
  ACDSetInitial();
  gtk_widget_show_all(dialog);
}

void ACDSetInitial(void)
{
  if (actor->IsReady() == false)  return;
  if (actor->GetNumAction() == 1) {
    GtkWidget *mes;
    mes = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK, _("This act has only one action."));
    gtk_dialog_run(GTK_DIALOG(mes));
    gtk_widget_destroy(mes);
    return;
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnCCW), (gboolean)true);
  ACDSetInitial_Combo();
  ACDSetInitial_Radio(0, true);
  ACDSetInitial_Radio(0, false);
}

void ACDSetInitial_Combo(void)
{
  GtkWidget* opts[4] = {optAC11, optAC12, optAC2, optAC3};

  int numact8 = actor->GetNumAction() / 8;
  if (actor->GetNumAction() % 8)  numact8++;

  char buf[20];
  GtkWidget *menu;
  GtkWidget *menui;
  for (int i=0; i<4; i++) {
    gtk_option_menu_remove_menu(GTK_OPTION_MENU(opts[i]));
    menu = gtk_menu_new();
    for (int j=0; j<numact8; j++) {
      SNPRINTF(buf, sizeof(buf), "act%02d", j);
      menui = gtk_menu_item_new_with_label(buf);
      gtk_widget_show(menui);
      gtk_container_add(GTK_CONTAINER(menu), menui);
    }
    gtk_option_menu_set_menu(GTK_OPTION_MENU(opts[i]), menu);
  }
}

void ACDSetInitial_Radio(int actno, bool isfrom)
{
  GtkWidget **baseR = isfrom ? rbtnAC[0] : rbtnAC[1];

  for (int i = 0; i <= 7; i++) {
    gtk_widget_set_sensitive(*(baseR+i), (gboolean)true);
  }

  int n = actor->GetNumAction() - actno * 8;
  switch (n) {
  case 1:
    for (int i = 1; i <= 7; i++) {
      gtk_widget_set_sensitive(*(baseR+i), (gboolean)false);
    }
    break;
  case 2:
    gtk_widget_set_sensitive(*baseR, (gboolean)false);
    for (int i = 2; i <= 6; i++) {
      gtk_widget_set_sensitive(*(baseR+i), (gboolean)false);
    }
    break;
  case 4:
    for (int i = 0; i <= 7; i++) {
      if (i % 2 == 0) {
        gtk_widget_set_sensitive(*(baseR+i), (gboolean)false);
      } else {
        gtk_widget_set_sensitive(*(baseR+i), (gboolean)true);
      }
    }
    break;
  default:
    for (int i = n; i <= 7; i++) {
      gtk_widget_set_sensitive(*(baseR+i), (gboolean)false);
    }
    break;
  }
}

void ACDDetailComboChange(GtkOptionMenu *optionmenu, gpointer user_data)
{
  if (actor->IsReady() == false)  return;

  GtkWidget *menu, *active_item;
  menu = GTK_OPTION_MENU(optionmenu)->menu;
  active_item = gtk_menu_get_active(GTK_MENU(menu));
  int i = g_list_index(GTK_MENU_SHELL(menu)->children, active_item);
  int ID = (int)user_data;

  if (ID == 11) {
    ACDSetInitial_Radio(i, true);
  } else {
    ACDSetInitial_Radio(i, false);
  }
  if (ID == 11 || ID == 12) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnDETAIL), (gboolean)true);
  } else if (ID == 2) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnCW), (gboolean)true);
  } else if (ID == 3) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbtnCCW), (gboolean)true);
  }
}

void ACDProcAny(void)
{
  if (actor->IsActReady() == false)  return;
  if (autoPlay) {
    OnBtnStop(NULL, NULL);
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnCW))) { 
    ACDProcCopy4to8(true);
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnCCW))) {
    ACDProcCopy4to8(false);
  } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnDETAIL))) {
    ACDProcCopyDetail();
  }
  ProcPatternChange(true);
}

void ACDProcCopy4to8(bool cw)
{
  GtkWidget *opt = cw ? optAC2 : optAC3;

  GtkWidget *menu, *active_item;
  menu = GTK_OPTION_MENU(opt)->menu;
  active_item = gtk_menu_get_active(GTK_MENU(menu));
  int noact = g_list_index(GTK_MENU_SHELL(menu)->children, active_item);
  int numact = actor->GetNumAction() - noact * 8;
  if (numact < 8) {
    GtkWidget *mes;
    mes = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK, _("This act doesn't have 8 actions."));
    gtk_dialog_run(GTK_DIALOG(mes));
    gtk_widget_destroy(mes);
    return;
  }

  int baseact = noact * 8;
  for (int i=0; i<4; i++) {
    if (cw) {
      actor->CopyAction(baseact+i*2+1, baseact+i*2);
    } else {
      actor->CopyAction(baseact+i*2, baseact+i*2+1);
    }
  }
}

void ACDProcCopyDetail(void)
{
  GtkWidget *menu, *active_item;
  menu = GTK_OPTION_MENU(optAC11)->menu;
  active_item = gtk_menu_get_active(GTK_MENU(menu));
  int srcact;
  srcact = 8 * g_list_index(GTK_MENU_SHELL(menu)->children, active_item);
  int i;
  for (i = 0; i <= 7; i++) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnAC[0][i]))) {
      break;
    }
  }
  srcact += i;

  menu = GTK_OPTION_MENU(optAC12)->menu;
  active_item = gtk_menu_get_active(GTK_MENU(menu));
  int destact;
  destact = 8 * g_list_index(GTK_MENU_SHELL(menu)->children, active_item);
  for (i = 0; i <= 7; i++) {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rbtnAC[1][i]))) {
      break;
    }
  }
  destact += i;

  actor->CopyAction(destact, srcact);
}

void ACDBuildDialog(GtkWidget *dialog)
{
  GtkWidget *table1 = gtk_table_new(7, 3, FALSE);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table1, FALSE, FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 5);
  gtk_table_set_row_spacings(GTK_TABLE(table1), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table1), 5);

  GtkWidget *label1 = gtk_label_new(_("select below"));
  gtk_table_attach(GTK_TABLE(table1), label1, 0, 1, 0, 1,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

  GtkWidget *label2 = gtk_label_new(_("description"));
  gtk_table_attach(GTK_TABLE(table1), label2, 2, 3, 0, 1,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

  rbtnCW = gtk_radio_button_new_with_mnemonic(NULL, _("copy clockwise"));
  gtk_table_attach(GTK_TABLE(table1), rbtnCW, 0, 1, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  GSList *rbtn1_group = NULL;
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnCW), rbtn1_group);
  rbtn1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnCW));

  rbtnCCW = gtk_radio_button_new_with_mnemonic(NULL, _("copy counter\nclockwise"));
  gtk_table_attach(GTK_TABLE(table1), rbtnCCW, 0, 1, 4, 5,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnCCW), rbtn1_group);
  rbtn1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnCCW));

  rbtnDETAIL = gtk_radio_button_new_with_mnemonic(NULL, _("copy detail"));
  gtk_table_attach(GTK_TABLE(table1), rbtnDETAIL, 0, 1, 6, 7,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnDETAIL), rbtn1_group);
  rbtn1_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnDETAIL));

  GtkWidget *hbox1 = gtk_hbox_new(FALSE, 5);
  gtk_table_attach(GTK_TABLE(table1), hbox1, 2, 3, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  optAC2 = gtk_option_menu_new();
  gtk_box_pack_start(GTK_BOX(hbox1), optAC2, FALSE, FALSE, 0);

  GtkWidget *label3 = gtk_label_new(_("1.from south direction\n  to southwest direction\n2.from west direction\n  to northwest direction\n...\n"));
  gtk_box_pack_start(GTK_BOX(hbox1), label3, FALSE, TRUE, 0);

  GtkWidget *hbox2 = gtk_hbox_new(FALSE, 5);
  gtk_table_attach(GTK_TABLE(table1), hbox2, 2, 3, 4, 5,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  optAC3 = gtk_option_menu_new();
  gtk_box_pack_start(GTK_BOX(hbox2), optAC3, FALSE, TRUE, 0);

  GtkWidget *label4 = gtk_label_new(_("1.from southwest direction\n  to south direction\n2.from northwest direction\n  to west direction\n...\n"));
  gtk_box_pack_start(GTK_BOX(hbox2), label4, FALSE, TRUE, 0);

  GtkWidget *hseparator7 = gtk_hseparator_new();
  gtk_table_attach(GTK_TABLE(table1), hseparator7, 2, 3, 3, 4,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *hseparator3 = gtk_hseparator_new();
  gtk_table_attach(GTK_TABLE(table1), hseparator3, 0, 1, 1, 2,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *hseparator4 = gtk_hseparator_new();
  gtk_table_attach(GTK_TABLE(table1), hseparator4, 2, 3, 1, 2,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *hseparator9 = gtk_hseparator_new();
  gtk_table_attach(GTK_TABLE(table1), hseparator9, 0, 1, 3, 4,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *vseparator5 = gtk_vseparator_new();
  gtk_table_attach(GTK_TABLE(table1), vseparator5, 1, 2, 0, 1,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *vseparator2 = gtk_vseparator_new();
  gtk_table_attach(GTK_TABLE(table1), vseparator2, 1, 2, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *vseparator3 = gtk_vseparator_new();
  gtk_table_attach(GTK_TABLE(table1), vseparator3, 1, 2, 4, 5,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *vseparator4 = gtk_vseparator_new();
  gtk_table_attach(GTK_TABLE(table1), vseparator4, 1, 2, 6, 7,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *hseparator11 = gtk_hseparator_new();
  gtk_table_attach(GTK_TABLE(table1), hseparator11, 2, 3, 5, 6,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *hseparator12 = gtk_hseparator_new();
  gtk_table_attach(GTK_TABLE(table1), hseparator12, 0, 1, 5, 6,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *hbox3 = gtk_hbox_new(FALSE, 8);
  gtk_table_attach(GTK_TABLE(table1), hbox3, 2, 3, 6, 7,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *table2 = gtk_table_new(2, 2, FALSE);
  gtk_box_pack_start(GTK_BOX(hbox3), table2, TRUE, TRUE, 0);
  gtk_table_set_row_spacings(GTK_TABLE(table2), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table2), 5);

  GtkWidget *label5 = gtk_label_new(_("From:"));
  gtk_table_attach(GTK_TABLE(table2), label5, 0, 1, 0, 1,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  optAC11 = gtk_option_menu_new();
  gtk_table_attach(GTK_TABLE(table2), optAC11, 1, 2, 0, 1,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *table4 = gtk_table_new(3, 3, FALSE);
  gtk_table_attach(GTK_TABLE(table2), table4, 1, 2, 1, 2,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), 0, 0);

  GSList *rbtn0_group = NULL;
  for (int i=0; i<3; i++) {
    rbtnAC[0][i+3] = gtk_radio_button_new_with_mnemonic(NULL, "");
    gtk_table_attach(GTK_TABLE(table4), rbtnAC[0][i+3], i, i+1, 0, 1,
      (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[0][i+3]), rbtn0_group);
    rbtn0_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[0][i+3]));
  }
  rbtnAC[0][2] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table4), rbtnAC[0][2], 0, 1, 1, 2,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[0][2]), rbtn0_group);
  rbtn0_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[0][2]));

  rbtnAC[0][6] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table4), rbtnAC[0][6], 2, 3, 1, 2,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[0][6]), rbtn0_group);
  rbtn0_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[0][6]));

  rbtnAC[0][1] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table4), rbtnAC[0][1], 0, 1, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[0][1]), rbtn0_group);
  rbtn0_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[0][1]));

  rbtnAC[0][0] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table4), rbtnAC[0][0], 1, 2, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[0][0]), rbtn0_group);
  rbtn0_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[0][0]));

  rbtnAC[0][7] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table4), rbtnAC[0][7], 2, 3, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[0][7]), rbtn0_group);
  rbtn0_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[0][7]));

  GtkWidget *table3 = gtk_table_new(2, 2, FALSE);
  gtk_box_pack_start(GTK_BOX(hbox3), table3, TRUE, TRUE, 0);
  gtk_table_set_row_spacings(GTK_TABLE(table3), 5);
  gtk_table_set_col_spacings(GTK_TABLE(table3), 5);

  GtkWidget *label6 = gtk_label_new(_("To:"));
  gtk_table_attach(GTK_TABLE(table3), label6, 0, 1, 0, 1,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  optAC12 = gtk_option_menu_new();
  gtk_table_attach(GTK_TABLE(table3), optAC12, 1, 2, 0, 1,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GtkWidget *table5 = gtk_table_new(3, 3, FALSE);
  gtk_table_attach(GTK_TABLE(table3), table5, 1, 2, 1, 2,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  GSList *rbtn10_group = NULL;
  for (int i=0; i<3; i++) {
    rbtnAC[1][i+3] = gtk_radio_button_new_with_mnemonic(NULL, "");
    gtk_table_attach(GTK_TABLE(table5), rbtnAC[1][i+3], i, i+1, 0, 1,
      (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
    gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[1][i+3]), rbtn10_group);
    rbtn10_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[1][i+3]));
  }
  rbtnAC[1][2] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table5), rbtnAC[1][2], 0, 1, 1, 2,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[1][2]), rbtn10_group);
  rbtn10_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[1][2]));

  rbtnAC[1][6] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table5), rbtnAC[1][6], 2, 3, 1, 2,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[1][6]), rbtn10_group);
  rbtn10_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[1][6]));

  rbtnAC[1][1] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table5), rbtnAC[1][1], 0, 1, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[1][1]), rbtn10_group);
  rbtn10_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[1][1]));

  rbtnAC[1][0] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table5), rbtnAC[1][0], 1, 2, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[1][0]), rbtn10_group);
  rbtn10_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[1][0]));

  rbtnAC[1][7] = gtk_radio_button_new_with_mnemonic(NULL, "");
  gtk_table_attach(GTK_TABLE(table5), rbtnAC[1][7], 2, 3, 2, 3,
    (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_radio_button_set_group(GTK_RADIO_BUTTON(rbtnAC[1][7]), rbtn10_group);
  rbtn10_group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbtnAC[1][7]));
}

void ACDResponse(GtkDialog *dialog, gint arg1, gpointer user_data)
{
  if (arg1 == GTK_RESPONSE_APPLY) {
    ACDProcAny();
  } else {
    gtk_widget_destroy(GTK_WIDGET(user_data));
  }
}

