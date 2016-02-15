#include <stdlib.h>
#include <string.h>

#include "entry.h"

void SetInterval(int inter)
{
  char tmp[20];

  GtkEntry *entInter = (GtkEntry *)glade_xml_get_widget(windowXML, "entInter");
  SNPRINTF(tmp, sizeof(tmp), "%d", inter);
  gtk_entry_set_text(entInter, tmp);
}

LIBGLADE_SIGNAL_FUNC void OnEntryIntervalActivate(GtkEntry *entry, gpointer user_data)
{
  char tmp[20];
  GtkEntry *entInter = (GtkEntry *)glade_xml_get_widget(windowXML, "entInter");

  strncpy(tmp, gtk_entry_get_text(entInter), sizeof(tmp));
  actor->SetInterval(actor->curAct, (float)atof(tmp));
}

