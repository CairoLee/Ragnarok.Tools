#include "scroll.h"

int GetScrollImgPos(void)
{
  GtkRange *scrImage = (GtkRange *)glade_xml_get_widget(windowXML, "scrImage");
  return (int)GTK_ADJUSTMENT(scrImage->adjustment)->value;
}

void SetScrollPos_(GtkWidget *scr, int pos, int max)
{
  GTK_ADJUSTMENT(GTK_RANGE(scr)->adjustment)->lower = 0;
  GTK_ADJUSTMENT(GTK_RANGE(scr)->adjustment)->upper = max+1;
  GTK_ADJUSTMENT(GTK_RANGE(scr)->adjustment)->value = pos;
  GTK_ADJUSTMENT(GTK_RANGE(scr)->adjustment)->page_size = 1;

  gtk_adjustment_value_changed(GTK_ADJUSTMENT(GTK_RANGE(scr)->adjustment));
}

LIBGLADE_SIGNAL_FUNC void AdjustBoundsScrFrame(GtkRange *range, gdouble arg1, gpointer user_data)
{
  if (actor->IsActReady() == false)  return;
  UnSelectSelection();
  actor->curPat = (int)arg1;
  int max = actor->GetNumPattern(actor->curAct) - 1;
  if (actor->curPat > max)  actor->curPat = 0;
  if (actor->curPat < 0)  actor->curPat = max;

  ProcPatternChange(true);
}

LIBGLADE_SIGNAL_FUNC gboolean ReleaseScrFrame(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  if (actor->IsActReady() == false)  return FALSE;
  UnSelectSelection();
  //SetScrollPos_(scrFrame, actor->curPat, actor->GetNumPattern(actor->curAct)-1);
  SetScrollPos_(widget, actor->curPat, actor->GetNumPattern(actor->curAct)-1);
  ProcPatternChange(true);
  return FALSE;
}

LIBGLADE_SIGNAL_FUNC void ChangedScrImage(GtkRange *range, gpointer user_data)
{
  if (actor->IsSprReady() == false)  return;
  actor->curImg = GetScrollImgPos();
  //SetScrollPos_(scrImage, actor->curImg, actor->GetNumImage()-1);
  SetImages(actor->curImg);
}
