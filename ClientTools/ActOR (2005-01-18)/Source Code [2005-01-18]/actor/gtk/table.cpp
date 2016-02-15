#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"

int cellHeight;

DWORD HexToDWORD(char *hex)
{
  DWORD ret = 0;
  while (*hex != '\0') {
    if (*hex >= 'a') *hex -= 'a' - 'A';
    if (*hex >= 'A') {
      *hex = *hex - 'A' + 10;
      if (*hex >= 16)  return 0;
    } else {
      *hex -= '0';
      if (*hex >= 10)  return 0;
    }
    ret *= 16;
    ret |= *hex;
    hex++;
  }
  return ret;
}

BYTE SaturateByte5(BYTE orig, bool plus)
{
  BYTE ret;

  if (plus) {
    if (orig <= 250) {
      ret = orig + 5;
    } else {
      ret = 255;
    }
  } else {
    if (orig >= 5) {
      ret = orig - 5;
    } else {
      ret = 0;
    }
  }
  return ret;
}

void GetSelectedTableItem(int *sel)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkTreeModel *model = gtk_tree_view_get_model(table);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(table);
  GtkTreeIter iter;

  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return;
  int i=0, j=0;
  do {
    if (gtk_tree_selection_iter_is_selected(selection, &iter)) {
      *(sel + j) = i;
      j++;
    }
    i++;
  } while (gtk_tree_model_iter_next(model, &iter));
}

void SetTableFromEdit(char *str, int row, int column)
{
  actor->SetModified();

  char txt[128];
  int i;
  float f;

  if (row == actor->GetNumSprites(actor->curAct, actor->curPat)) {
    //if head
    i = atoi(str);
    switch (column) {
    case 2:  //xoffs
      actor->SetExtXValue(actor->curAct, actor->curPat, i);
      break;
    case 3:  //yoffs;
      actor->SetExtYValue(actor->curAct, actor->curPat, i);
      break;
    }
    SNPRINTF(txt, sizeof(txt), "%d", i);
    SetTableItem(txt, row, column);
    ProcPatternChange(true);
    return;
  }

  switch (column) {
  case 0:  //SprNo
    i = atoi(str);
    if (actor->GetSpTypeValue(actor->curAct, actor->curPat, row) == 0) {
      int max = actor->GetNumPalImage();
      if (i >= max)  i = max - 1;
    } else {
      int max = actor->GetNumFlatImage();
      if (i >= max)  i = max - 1;
    }
    SNPRINTF(txt, sizeof(txt), "%d", i);
    actor->SetSprNoValue(actor->curAct, actor->curPat, row, i);
    break;
  case 1:  //SpType
    i = atoi(str);
    if (i > 1)  i = 1;
    if (actor->GetNumFlatImage() == 0)  i = 0;
    SNPRINTF(txt, sizeof(txt), "%d", i);
    actor->SetSpTypeValue(actor->curAct, actor->curPat, row, i);
    break;
  case 2:  //Xoffs
    i = atoi(str);
    SNPRINTF(txt, sizeof(txt), "%d", i);
    actor->SetXOffsValue(actor->curAct, actor->curPat, row, i);
    break;
  case 3:  //Yoffs
    i = atoi(str);
    SNPRINTF(txt, sizeof(txt), "%d", i);
    actor->SetYOffsValue(actor->curAct, actor->curPat, row, i);
    break;
  case 4:  //Mirror
    i = atoi(str);
    i = (i != 0) ? 1 : 0;
    SNPRINTF(txt, sizeof(txt), "%d", i);
    actor->SetMirrorValue(actor->curAct, actor->curPat, row, i);
    break;
  case 5:  //AABBGGRR
    i = HexToDWORD(str);
    SNPRINTF(txt, sizeof(txt), "%08X", i);
    actor->SetABGRValue(actor->curAct, actor->curPat, row, i);
    break;
  case 6: //Xmag
    f = (float)atof(str);
    SNPRINTF(txt, sizeof(txt), "%f", f);
    actor->SetXMagValue(actor->curAct, actor->curPat, row, f);
    break;
  case 7: //Ymag
    f = (float)atof(str);
    SNPRINTF(txt, sizeof(txt), "%f", f);
    actor->SetYMagValue(actor->curAct, actor->curPat, row, f);
    break;
  case 8: //rot
    i = atoi(str);
    i = i%360;
    if (i < 0) i += 360;
    SNPRINTF(txt, sizeof(txt), "%d", i);
    actor->SetRotValue(actor->curAct, actor->curPat, row, i);
    break;
  }
  SetTableItem(txt, row, column);
  ProcPatternChange(true);
}

void SetTableItemAll(int noAct, int noPat)
{
  if (IsActReady() == false)  return;

  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkListStore *store = (GtkListStore *)gtk_tree_view_get_model(table);
  char buf[20];
  int max = actor->GetNumSprites(noAct, noPat);
  if (actor->matchRefHead && actor->enableRefHead)  max++;
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  int *selArray;
  if (!autoPlay) {
    selArray = (int *)malloc(sizeof(int)*max);
    GetSelectedTableItem(selArray);
  }
  gtk_list_store_clear(store);

  int i;
  for (i=0; i< actor->GetNumSprites(noAct, noPat); i++) {
    // colheader = {"SprNo", "SpType", "Xoffs", "Yoffs", "Mir", "AABBGGRR", "Xmag", "Ymag", "Rot"};
    //spr no
    SNPRINTF(buf, sizeof(buf), "%d", actor->GetSprNoValue(noAct, noPat, i));
    SetTableItem(buf, i, 0);
    //sp type
    SNPRINTF(buf, sizeof(buf), "%d", actor->GetSpTypeValue(noAct, noPat, i));
    SetTableItem(buf, i, 1);
    int xo, yo;
    actor->GetOffsPoint(&xo, &yo, noAct, noPat, i);
    // xoffs
    SNPRINTF(buf, sizeof(buf), "%d", xo);
    SetTableItem(buf, i, 2);
    // yoffs
    SNPRINTF(buf, sizeof(buf), "%d", yo);
    SetTableItem(buf, i, 3);
    // mirror
    SNPRINTF(buf, sizeof(buf), "%d", actor->GetMirrorValue(noAct, noPat, i));
    SetTableItem(buf, i, 4);
    // saturation
    SNPRINTF(buf, sizeof(buf), "%08X", actor->GetABGRValue(noAct, noPat, i));
    SetTableItem(buf, i, 5);
    // xmag
    SNPRINTF(buf, sizeof(buf), "%f", actor->GetXMagValue(noAct, noPat, i));
    SetTableItem(buf, i, 6);
    // ymag
    SNPRINTF(buf, sizeof(buf), "%f", actor->GetYMagValue(noAct, noPat, i));
    SetTableItem(buf, i, 7);
    // rot
    SNPRINTF(buf, sizeof(buf), "%d", actor->GetRotValue(noAct, noPat, i));
    SetTableItem(buf, i, 8);
  }
  if (actor->matchRefHead && actor->enableRefHead) {
    SetTableItem("EXT(ref head)", i, 0);
    int extX, extY;
    actor->GetExtOffsPoint(&extX, &extY, actor->curAct, actor->curPat);
    SNPRINTF(buf, sizeof(buf), "%d", extX);
    SetTableItem(buf, i, 2);
    SNPRINTF(buf, sizeof(buf), "%d", extY);
    SetTableItem(buf, i, 3);
  }
  if (!autoPlay) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(table));
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(table));
    gtk_tree_model_get_iter_first(model, &iter);
    int j=0;
    if (numS) {
      for (int i=0; i<max; i++, gtk_tree_model_iter_next(model, &iter)) {
        if (i == *(selArray+j)) {
          gtk_tree_selection_select_iter(selection, &iter);
          j++;
        }
      }
    }
    free(selArray);
  }
}

void SetTableItem(char *txt, int row, int column)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkListStore *store = (GtkListStore *)gtk_tree_view_get_model(table);
  GtkTreeModel *model = gtk_tree_view_get_model(table);
  GtkTreeIter iter;
  gboolean isexist;

  isexist = gtk_tree_model_get_iter_first(model, &iter);
  if (column == 0) {
    if (isexist) {
      for (int i=0; i<row; i++) {
        isexist = gtk_tree_model_iter_next(model, &iter);
        if (isexist == FALSE)  break;
      }
    }
    if (isexist) {
      gtk_list_store_set(store, &iter, column, txt, -1);
    } else {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, column, txt, EDITABLE_COLUMN, TRUE, -1);
    }
  } else {
    for (int i=0; i<row; i++)  gtk_tree_model_iter_next(model, &iter);
    gtk_list_store_set(store, &iter, column, txt, -1);
  }
}

void SwapTableItemText(int max, int rowA, int rowB)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkListStore *store = (GtkListStore *)gtk_tree_view_get_model(table);
  GtkTreeModel *model = gtk_tree_view_get_model(table);
  GtkTreeIter iter, iterA, iterB;

  gtk_tree_model_get_iter_first(model, &iter);

  for (int i=0; i<max; i++) {
    if (rowA == i)  iterA = iter;
    if (rowB == i)  iterB = iter;
  }
  gtk_list_store_swap(store, &iterA, &iterB);
}

void SelectUnselectRow(int selno, int unselno)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkTreeModel *model = gtk_tree_view_get_model(table);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(table);
  GtkTreeIter iter;

  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return;
  int i=0;
  do {
    if (i == selno) {
      gtk_tree_selection_select_iter(selection, &iter);
    }
    if (i== unselno) {
      gtk_tree_selection_unselect_iter(selection, &iter);
    }
    i++;
  } while (gtk_tree_model_iter_next(model, &iter));
}

void DownSelectedTableItem(int num)
{
  if (DispUndoInitWarningSPO() == false)  return;
  UndoRedoInit();
  int max = actor->GetNumSprites(actor->curAct, actor->curPat);
  if (actor->enableRefHead)  max++;
  int *selArray;
  selArray = (int *)malloc(sizeof(int)*max);
  GetSelectedTableItem(selArray);

  int i=0;
  if (actor->enableRefHead && actor->matchRefHead) {
    if (*(selArray+num-1) == max-1) { //if head row select
      num--;
    }
    if (*(selArray+num-1) == max-2) { //if (head row-1) select
      num--;
    }
  } else {
    while (*(selArray+num-1-i) == max-1-i) { //if last one selected, it is ignored
      i++;
    }
    num -= i;
  }

  int spA, spB;
  for (i=num-1; i>=0; i--) {
    spA = *(selArray+i);
    spB = *(selArray+i)+1;
    if (spA < 0 || spA >= actor->GetNumSprites(actor->curAct, actor->curPat))  continue;
    if (spB < 0 || spB >= actor->GetNumSprites(actor->curAct, actor->curPat))  continue;
    SwapTableItemText(max, spB, spA);
    actor->SwapSpriteOrder(actor->curAct, actor->curPat, spA, spB);
    SelectUnselectRow(spB, spA);
  }
  free(selArray);
}

void UpSelectedTableItem(int num)
{
  if (DispUndoInitWarningSPO() == false)  return;
  UndoRedoInit();
  int max = actor->GetNumSprites(actor->curAct, actor->curPat);
  if (actor->enableRefHead)  max++;
  int *selArray;
  selArray = (int *)malloc(sizeof(int)*max);
  GetSelectedTableItem(selArray);

  if (actor->enableRefHead && actor->matchRefHead) {
    if (*(selArray+num-1) == max-1) { //if head row select
      num--;
    }
  }
  int i=0;
  while (*(selArray+i) == i) { //if first one selected, it is ignored
    i++;
  }
  num -= i;

  int spA, spB;
  for (i=0; i<num; i++) {
    spA = *(selArray+i)-1;
    spB = *(selArray+i);
    if (spA < 0 || spA >= actor->GetNumSprites(actor->curAct, actor->curPat))  continue;
    if (spB < 0 || spB >= actor->GetNumSprites(actor->curAct, actor->curPat))  continue;
    SwapTableItemText(max, spA, spB);
    actor->SwapSpriteOrder(actor->curAct, actor->curPat, spA, spB);
    SelectUnselectRow(spA, spB);
  }

  free(selArray);
}

void DeleteSelectedSpriteInTable(void)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  int max = actor->GetNumSprites(actor->curAct, actor->curPat);
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  int *selArray = NULL;
  if (!autoPlay) {
    selArray = (int *)malloc(sizeof(int)*max);
    GetSelectedTableItem(selArray);
  }

  if (*(selArray+numS-1) == actor->GetNumSprites(actor->curAct, actor->curPat))  numS--;
  for (int i=numS-1; i>=0; i--) {
    actor->DelSpr(actor->curAct, actor->curPat, *(selArray+i));
  }

  free(selArray);

  ProcPatternChange(true);
}

void UnSelectSelection(void)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  gtk_tree_selection_unselect_all(gtk_tree_view_get_selection(table));
}

void EditedTable(GtkCellRendererText *cell, const gchar *path_string, const gchar *new_test, gpointer data)
{
  if (actor->dragState == DRAG_TABLE_ROW)  return;
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreePath *path = gtk_tree_path_new_from_string(path_string);
  GtkTreeIter iter;
                                                                                
  gint column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
  gtk_tree_model_get_iter(model, &iter, path);
  gint i;
  i = gtk_tree_path_get_indices(path)[0];

  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  if (gtk_tree_selection_path_is_selected(gtk_tree_view_get_selection(table), path)) {
    SetTableFromEdit((char *)new_test, i, column);
  }
}

LIBGLADE_SIGNAL_FUNC void ChangeSaturationABGRValue(GtkWidget *menuitem, gpointer user_data)
{
  int data;
  if (strcmp(glade_get_widget_name(menuitem), "menuRPlus") == 0) {
    data = 0x0001;
  } else if (strcmp(glade_get_widget_name(menuitem), "menuRMinus") == 0) {
    data = 0x0000;
  } else if (strcmp(glade_get_widget_name(menuitem), "menuGPlus") == 0) {
    data = 0x0101;
  } else if (strcmp(glade_get_widget_name(menuitem), "menuGMinus") == 0) {
    data = 0x0100;
  } else if (strcmp(glade_get_widget_name(menuitem), "menuBPlus") == 0) {
    data = 0x0201;
  } else if (strcmp(glade_get_widget_name(menuitem), "menuBMinus") == 0) {
    data = 0x0200;
  } else if (strcmp(glade_get_widget_name(menuitem), "menuAPlus") == 0) {
    data = 0x0301;
  } else if (strcmp(glade_get_widget_name(menuitem), "menuAMinus") == 0) {
    data = 0x0300;
  }
  int type = (data >> 8) & 0xFF;
  bool plus = data & 1 ? true : false;
  gchar *buf;
  DWORD sat;
  BYTE v;
  DWORD mask, mask1, mask2;
  mask1 = 0xFFFFFF00;
  mask1 <<= type*8;
  mask2 = 0x00FFFFFF;
  mask2 >>= (3-type)*8;
  mask = mask1 | mask2;
  int max = actor->GetNumSprites(actor->curAct, actor->curPat);
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkTreeModel *model = gtk_tree_view_get_model(table);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(table);
  GtkTreeIter iter;

  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return;
  for (int i=0; i<max; i++) {
    if (gtk_tree_selection_iter_is_selected(selection, &iter)) {
      gtk_tree_model_get(model, &iter, SAT_COLUMN, &buf, -1);
      // AABBGGRR
      sat = HexToDWORD(buf);
      v = (BYTE)((sat & (~mask)) >> (type*8));
      if (plus) {
        v = SaturateByte5(v, true);
      } else {
        v = SaturateByte5(v, false);
      }
      sat &= mask;
      sat |= v << (type*8);
      sprintf(buf, "%08X", sat);
      SetTableFromEdit(buf, i, SAT_COLUMN);
      // iter is broken
      if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return;
      for (int j=0; j<i; j++)  gtk_tree_model_iter_next(model, &iter);
    }
    if (gtk_tree_model_iter_next(model, &iter) == FALSE)  break;
  }
}

LIBGLADE_SIGNAL_FUNC void ChangeMirror(GtkMenuItem *menuitem, gpointer user_data)
{
  gchar* buf;
  int max = actor->GetNumSprites(actor->curAct, actor->curPat);
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkTreeModel *model = gtk_tree_view_get_model(table);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(table);
  GtkTreeIter iter;

  gtk_tree_model_get_iter_first(model, &iter);
  for (int i=0; i<max; i++) {
    if (gtk_tree_selection_iter_is_selected(selection, &iter)) {
      gtk_tree_model_get(model, &iter, MIRROR_COLUMN, &buf, -1);
      if (buf[0] == '1') {
        buf[0] = '0';
      } else {
        buf[0] = '1';
      }
      SetTableFromEdit(buf, i, MIRROR_COLUMN);
      // iter is broken
      if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return;
      for (int j=0; j<i; j++)  gtk_tree_model_iter_next(model, &iter);
    }
    if (gtk_tree_model_iter_next(model, &iter) == FALSE)  break;
  }
}

LIBGLADE_SIGNAL_FUNC gboolean motionTable(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  actor->newMP.x = (int)event->x;
  actor->newMP.y = (int)event->y;
  if (actor->dragState == DRAG_TABLE_ROW) {
    GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
    int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
    if (numS == 0)  return FALSE;
    if (actor->newMP.y - actor->oldMP.y > cellHeight) {
      DownSelectedTableItem(numS);
      actor->oldMP.y = actor->newMP.y;
      SetTableItemAll(actor->curAct, actor->curPat);
    } else if (actor->newMP.y - actor->oldMP.y < -cellHeight) {
      UpSelectedTableItem(numS);
      actor->oldMP.y = actor->newMP.y;
      SetTableItemAll(actor->curAct, actor->curPat);
    }
  }
 
  return FALSE;
}

LIBGLADE_SIGNAL_FUNC gboolean bpressTable(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkTreeViewColumn *col = gtk_tree_view_get_column(table, 0);
  GdkRectangle rect;

  gtk_tree_view_get_cell_area(table, NULL, col, &rect);
  
  int xoff, yoff, w, h;
  gtk_tree_view_column_cell_get_size(col, &rect, &xoff, &yoff, &w, &h);
  cellHeight = h;

  SetMousePoint(event);
  if (actor->dragState == 0)  actor->dragState = DRAG_TABLE_ROW;

  return FALSE;
}

LIBGLADE_SIGNAL_FUNC gboolean breleaseTable(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  actor->dragState = 0;

  return FALSE;
}
