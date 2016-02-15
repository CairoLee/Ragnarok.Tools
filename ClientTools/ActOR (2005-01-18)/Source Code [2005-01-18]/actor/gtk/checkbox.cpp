#include "checkbox.h"

LIBGLADE_SIGNAL_FUNC void OnBtnHead(GtkToggleButton *togglebutton, gpointer user_data)
{
  GtkWidget *frameHead = glade_xml_get_widget(windowXML, "frameHead");
  if (actor->enableRefHead) {
    actor->enableRefHead = false;
    gtk_widget_set_sensitive(frameHead, FALSE);
  } else {
    actor->enableRefHead = true;
    gtk_widget_set_sensitive(frameHead, TRUE);
  }
  ProcPatternChange(true);
}

LIBGLADE_SIGNAL_FUNC void OnBtnBody(GtkToggleButton *togglebutton, gpointer user_data)
{
  GtkWidget *frameBody = glade_xml_get_widget(windowXML, "frameBody");
  if (actor->enableRefBody) {
    actor->enableRefBody = false;
    gtk_widget_set_sensitive(frameBody, FALSE);
  } else {
    actor->enableRefBody = true;
    gtk_widget_set_sensitive(frameBody, TRUE);
  }
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void OnBtnEtc(GtkToggleButton *togglebutton, gpointer user_data)
{
  GtkWidget *frameEtc = glade_xml_get_widget(windowXML, "frameEtc");
  if (actor->enableRefEtc) {
    actor->enableRefEtc = false;
    gtk_widget_set_sensitive(frameEtc, FALSE);
  } else {
    actor->enableRefEtc = true;
    gtk_widget_set_sensitive(frameEtc, TRUE);
  }
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void OnBtnNeigh(GtkToggleButton *togglebutton, gpointer user_data)
{
  GtkWidget *frameNeigh = glade_xml_get_widget(windowXML, "frameNeigh");
  if (actor->enableRefNeigh) {
    actor->enableRefNeigh = false;
    gtk_widget_set_sensitive(frameNeigh, FALSE);
  } else {
    actor->enableRefNeigh = true;
    gtk_widget_set_sensitive(frameNeigh, TRUE);
  }
  DrawFrameAll();
}

void CheckRefHead(void)
{
  GtkWidget *chkbHead = glade_xml_get_widget(windowXML, "chkbHead");
  if (actor->sprRefHead->IsReady() == false) {
    gtk_widget_set_sensitive(chkbHead, FALSE);
  } else {
    gtk_widget_set_sensitive(chkbHead, TRUE);
  }

  GtkWidget *frameHead = glade_xml_get_widget(windowXML, "frameHead");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkbHead))) {
    gtk_widget_set_sensitive(frameHead, TRUE);
  } else {
    gtk_widget_set_sensitive(frameHead, FALSE);
  }
}

void CheckRefBody(void)
{
  GtkWidget *chkbBody = glade_xml_get_widget(windowXML, "chkbBody");
  if (actor->sprRefBody->IsReady() == false) {
    gtk_widget_set_sensitive(chkbBody, FALSE);
  } else {
    gtk_widget_set_sensitive(chkbBody, TRUE);
  }

  GtkWidget *frameBody = glade_xml_get_widget(windowXML, "frameBody");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkbBody))) {
    gtk_widget_set_sensitive(frameBody, TRUE);
  } else {
    gtk_widget_set_sensitive(frameBody, FALSE);
  }
}

void CheckRefEtc(void)
{
  GtkWidget *chkbEtc = glade_xml_get_widget(windowXML, "chkbEtc");
  if (actor->sprRefEtc->IsReady() == false) {
    gtk_widget_set_sensitive(chkbEtc, FALSE);
  } else {
    gtk_widget_set_sensitive(chkbEtc, TRUE);
  }

  GtkWidget *frameEtc = glade_xml_get_widget(windowXML, "frameEtc");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkbEtc))) {
    gtk_widget_set_sensitive(frameEtc, TRUE);
  } else {
    gtk_widget_set_sensitive(frameEtc, FALSE);
  }
}

void CheckRefNeigh(void)
{
  GtkWidget *chkbNeigh = glade_xml_get_widget(windowXML, "chkbNeigh");
  if (actor->sprRefNeigh->IsReady() == false) {
    gtk_widget_set_sensitive(chkbNeigh, FALSE);
    EnableNeighDirWindow(FALSE);
  } else {
    gtk_widget_set_sensitive(chkbNeigh, TRUE);
    EnableNeighDirWindow(TRUE);
  }

  GtkWidget *frameNeigh = glade_xml_get_widget(windowXML, "frameNeigh");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkbNeigh))) {
    gtk_widget_set_sensitive(frameNeigh, TRUE);
  } else {
    gtk_widget_set_sensitive(frameNeigh, FALSE);
  }
}

