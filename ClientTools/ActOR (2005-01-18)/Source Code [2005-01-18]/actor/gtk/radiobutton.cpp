#include "radiobutton.h"

LIBGLADE_SIGNAL_FUNC void OnRBtnHead(GtkButton *button, gpointer user_data)
{
  GtkToggleButton *rbtnHeadFront = (GtkToggleButton *)glade_xml_get_widget(windowXML, "rbtnHeadFront");
  if (gtk_toggle_button_get_active(rbtnHeadFront)) {
    actor->prioRefSpr.head = true;
  } else {
    actor->prioRefSpr.head = false;
  }
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void OnRBtnBody(GtkButton *button, gpointer user_data)
{
  GtkToggleButton *rbtnBodyFront = (GtkToggleButton *)glade_xml_get_widget(windowXML, "rbtnBodyFront");
  if (gtk_toggle_button_get_active(rbtnBodyFront)) {
    actor->prioRefSpr.body = true;
  } else {
    actor->prioRefSpr.body = false;
  }
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void OnRBtnEtc(GtkButton *button, gpointer user_data)
{
  GtkToggleButton *rbtnEtcFront = (GtkToggleButton *)glade_xml_get_widget(windowXML, "rbtnEtcFront");
  if (gtk_toggle_button_get_active(rbtnEtcFront)) {
    actor->prioRefSpr.etc = true;
  } else {
    actor->prioRefSpr.etc = false;
  }
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void OnRBtnNeigh(GtkButton *button, gpointer user_data)
{
  GtkToggleButton *rbtnNeighFront = (GtkToggleButton *)glade_xml_get_widget(windowXML, "rbtnNeighFront");
  if (gtk_toggle_button_get_active(rbtnNeighFront)) {
    actor->prioRefSpr.neigh = true;
  } else {
    actor->prioRefSpr.neigh = false;
  }
  DrawFrameAll();
}
