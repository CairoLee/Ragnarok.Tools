#include <string.h>

#include "buttons.h"

bool autoPlay;

void SetStateDirecButton(BYTE stat)
{
  GtkWidget *btnDirS = glade_xml_get_widget(windowXML, "btnDirS");
  gtk_widget_set_sensitive(btnDirS, (gboolean)(stat & 0x01));

  GtkWidget *btnDirSW = glade_xml_get_widget(windowXML, "btnDirSW");
  gtk_widget_set_sensitive(btnDirSW, (gboolean)(stat & 0x02));

  GtkWidget *btnDirW = glade_xml_get_widget(windowXML, "btnDirW");
  gtk_widget_set_sensitive(btnDirW, (gboolean)(stat & 0x04));

  GtkWidget *btnDirNW = glade_xml_get_widget(windowXML, "btnDirNW");
  gtk_widget_set_sensitive(btnDirNW, (gboolean)(stat & 0x08));

  GtkWidget *btnDirN = glade_xml_get_widget(windowXML, "btnDirN");
  gtk_widget_set_sensitive(btnDirN, (gboolean)(stat & 0x10));

  GtkWidget *btnDirNE = glade_xml_get_widget(windowXML, "btnDirNE");
  gtk_widget_set_sensitive(btnDirNE, (gboolean)(stat & 0x20));

  GtkWidget *btnDirE = glade_xml_get_widget(windowXML, "btnDirE");
  gtk_widget_set_sensitive(btnDirE, (gboolean)(stat & 0x40));

  GtkWidget *btnDirSE = glade_xml_get_widget(windowXML, "btnDirSE");
  gtk_widget_set_sensitive(btnDirSE, (gboolean)(stat & 0x80));
}

void SetStateAutoPlay(void)
{
  GtkWidget *table = glade_xml_get_widget(windowXML, "table");
  GtkWidget *entInter = glade_xml_get_widget(windowXML, "entInter");
  GtkWidget *optSound = glade_xml_get_widget(windowXML, "optSound");

  if (autoPlay) {
    gtk_widget_set_sensitive(table, FALSE);
    gtk_widget_set_sensitive(entInter, FALSE);
    gtk_widget_set_sensitive(optSound, FALSE);
  } else {
    gtk_widget_set_sensitive(table, TRUE);
    gtk_widget_set_sensitive(entInter, TRUE);
    gtk_widget_set_sensitive(optSound, TRUE);
  }
}

void EnableNeighDirWindow(gboolean sw)
{
  GtkWidget *btnRDirS = glade_xml_get_widget(windowXML, "btnRDirS");
  gtk_widget_set_sensitive(btnRDirS, sw);

  GtkWidget *btnRDirSW = glade_xml_get_widget(windowXML, "btnRDirSW");
  gtk_widget_set_sensitive(btnRDirSW, sw);

  GtkWidget *btnRDirW = glade_xml_get_widget(windowXML, "btnRDirS");
  gtk_widget_set_sensitive(btnRDirW, sw);

  GtkWidget *btnRDirNW = glade_xml_get_widget(windowXML, "btnRDirNW");
  gtk_widget_set_sensitive(btnRDirNW, sw);

  GtkWidget *btnRDirN = glade_xml_get_widget(windowXML, "btnRDirN");
  gtk_widget_set_sensitive(btnRDirN, sw);

  GtkWidget *btnRDirNE = glade_xml_get_widget(windowXML, "btnRDirNE");
  gtk_widget_set_sensitive(btnRDirNE, sw);

  GtkWidget *btnRDirE = glade_xml_get_widget(windowXML, "btnRDirE");
  gtk_widget_set_sensitive(btnRDirE, sw);

  GtkWidget *btnRDirSE = glade_xml_get_widget(windowXML, "btnRDirSE");
  gtk_widget_set_sensitive(btnRDirSE, sw);
}

LIBGLADE_SIGNAL_FUNC void OnBtnDirectClick(GtkButton *button, gpointer user_data)
{
  if (IsActReady() == false)  return;

  bool tmpPlay = autoPlay;
  if (tmpPlay) {
    OnBtnStop(NULL, NULL);
  }
  
  char btn_name[][9] = {
    "btnDirS", "btnDirSW", "btnDirW", "btnDirNW", "btnDirN", "btnDirNE", "btnDirE", "btnDirSE"
  };
  int direc;
  for (direc = 0; direc < 7; direc++) {
    if (strcmp(btn_name[direc], glade_get_widget_name(GTK_WIDGET(button))) == 0) {
      break;
    }
  }

  UnSelectSelection();
  if (actor->GetNumAction() >= 8) {
    GtkOptionMenu *optAct = (GtkOptionMenu *)glade_xml_get_widget(windowXML, "optAct");
    GtkWidget *menu, *active_item;
    menu = optAct->menu;
    active_item = gtk_menu_get_active(GTK_MENU(menu));
    int i = g_list_index(GTK_MENU_SHELL(menu)->children, active_item);
    actor->curAct = i*8 + direc;
  } else {
    switch (actor->GetNumAction()) {
    case 2:
      if (direc == 1) {
        actor->curAct = 0;
      } else {
        actor->curAct = 1;
      }
      break;
    case 4:
      actor->curAct = direc / 2;
      break;
    }
  }
  actor->curDir = direc;
  actor->curPat = 0;

  if (tmpPlay) {
    OnBtnPlay(NULL, NULL);
  }

  ProcPatternChange(true);
}

LIBGLADE_SIGNAL_FUNC void OnBtnPlay(GtkButton *button, gpointer user_data)
{
  GtkToggleButton *btnStop = (GtkToggleButton *)glade_xml_get_widget(windowXML, "btnStop");
  if (gtk_toggle_button_get_active(btnStop)) {
    gtk_toggle_button_set_active(btnStop, FALSE);
  }
  
  if (actor->IsActReady() && autoPlay == false) {
    autoPlay = true;
    SetStateAutoPlay();
    idTimer = gtk_timeout_add(actor->GetTimeInterval(), TimerFrameAll, NULL);
  }
}

LIBGLADE_SIGNAL_FUNC void OnBtnStop(GtkButton *button, gpointer user_data)
{
  GtkToggleButton *btnPlay = (GtkToggleButton *)glade_xml_get_widget(windowXML, "btnPlay");
  if (gtk_toggle_button_get_active(btnPlay)) {
    gtk_toggle_button_set_active(btnPlay, FALSE);
  }
  autoPlay = false;
  SetStateAutoPlay();
  if (idTimer) {
    gtk_timeout_remove(idTimer);
    idTimer = 0;
  }
}

LIBGLADE_SIGNAL_FUNC void OnBtnViewReset(GtkButton *button, gpointer user_data)
{
  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  CameraDefault(oglAllImg, &actor->cameraAll, &actor->widthAll, &actor->heightAll, &actor->ratioAll);
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void OnBtnNeiDirec(GtkButton *button, gpointer user_data)
{
  char btn_name[][10] = {
    "btnRDirS", "btnRDirSW", "btnRDirW", "btnRDirNW", "btnRDirN", "btnRDirNE", "btnRDirE", "btnRDirSE"
  };
  int direc;
  for (direc = 0; direc < 7; direc++) {
    if (strcmp(btn_name[direc], glade_get_widget_name(GTK_WIDGET(button))) == 0) {
      break;
    }
  }
  
  actor->ChangeNeighborPoint(direc);
  DrawFrameAll();
}
