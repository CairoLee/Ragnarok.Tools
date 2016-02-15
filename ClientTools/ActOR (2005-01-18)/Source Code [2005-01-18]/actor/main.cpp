#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#ifdef WIN32
#include <direct.h>
#include "../common/relocate/progname.h"
#include "../common/relocate/relocatable.h"
#else
#include <unistd.h>
#endif
#include <zlib.h>

#include <gdk/gdkkeysyms.h>

#include "main.h"

#define THIS_APP_NAME "actOR"

extern "C" {
  extern
#ifdef WIN32
  __declspec(dllimport)
#endif
  const char *locale_charset(void);
}
char NativeLANG[20];

CActor *actor;
bool agreement;
MOUSE_ASSIGN mouseAssign;
DWORD colorBG, colorSelect, colorLine;
int idTimer;
bool warningUndoFE;
bool warningUndoSPO;
bool ctrlKeyPressed;
bool shiftKeyPressed;
bool altKeyPressed;

#include "actor.glade.z"
#include "../EULA_comp.z"

GtkWidget *window;
GladeXML *windowXML;
GdkGLConfig *glconfig;

int main(int argc, char **argv)
{
  SetTextDomain(argv[0]);

  gtk_init(&argc, &argv);
  gtk_gl_init(&argc, &argv);


  glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_ALPHA | GDK_GL_MODE_DOUBLE));
  if (glconfig == NULL) {
      g_print("failed: OpenGL alpha blend\n");
      glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE));
    if (glconfig == NULL) {
      g_print("failed: OpenGL double buffer\n");
      glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA));
    }
  }

  actor = new CActor();
  actor->SetColors(colorBG, colorSelect, colorLine);
  autoPlay = true;
  BuildWindow();
  SignalConnect();

  GtkOptionMenu *optAct = (GtkOptionMenu *)glade_xml_get_widget(windowXML, "optAct");
  GtkWidget *menu = gtk_menu_new();
  gtk_option_menu_set_menu(optAct, menu);

  ReadPreference();
  if (agreement == false) {
    agreement = GetAgreement();
    if (agreement == false) {
      return 0;
    }
  }

  idTimer = 0;
  warningUndoFE = true;
  warningUndoSPO = true;
  UndoEnable(false);
  RedoEnable(false);

  gtk_widget_show_all(window);

  if (argc == 2) {
    OpenDialog_ReadFile(NULL, argv[1]);
  }
  OnBtnStop(NULL, NULL);
  gtk_main();

  WritePreference();

  return 0;
}

LIBGLADE_SIGNAL_FUNC void AuditAll(GtkMenuItem *menuitem, gpointer user_data)
{
  int actNo, patNo, sprId;
  int res;

    res = actor->AuditAll(&actNo, &patNo, &sprId);

    char mes[1024];
    char direc[20];
    char sdir[8][3] = {"S", "SW", "W", "NW", "N", "NE", "E", "SE"};
    SNPRINTF(direc, sizeof(direc), "%s", sdir[actNo % 8]);

    GtkWidget *md;

    switch (res) {
    case ACTOR_AUDIT_NOERROR:
      md = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
             GTK_BUTTONS_OK, _("No error and warning."));
      gtk_dialog_run(GTK_DIALOG(md));
      gtk_widget_destroy(md);
      break;
    case ACTOR_AUDIT_SPRMAXERROR:
      sprintf(mes, _("sprNo is over.\nAt act%02d direction %s frame %d sprite %d."),
        actNo / 8, _(direc), patNo, sprId);
      md = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
             GTK_BUTTONS_OK, mes);
      gtk_dialog_run(GTK_DIALOG(md));
      gtk_widget_destroy(md);
      break;
    case ACTOR_AUDIT_0205SPRWIDTHHEIGHTERROR:
      md = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
             GTK_BUTTONS_YES_NO, _("Width, heigh of sprite is not matching between .spr with .act.\nDo you want to modify correct?"));
      res = gtk_dialog_run(GTK_DIALOG(md));
      gtk_widget_destroy(md);
      if (res == GTK_RESPONSE_YES) {
        actor->ModifyCorrectSprWidthHeight();
      }
      break;
    case ACTOR_AUDIT_SOUNDMAXERROR:
      sprintf(mes, _("SoundNo is over.\nAt act%02d direction %s frame %d."),
        actNo / 8, _(direc), patNo);
      md = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
             GTK_BUTTONS_OK, mes);
      gtk_dialog_run(GTK_DIALOG(md));
      gtk_widget_destroy(md);
      break;
    }

}

LIBGLADE_SIGNAL_FUNC void OpenDialog_ReadFile(GtkMenuItem *menuitem, gpointer definedFilename)
{
  if (OnClose_Save(true) == false)  return;

  bool def = false;
  char fn[FILENAME_MAX];

  if (definedFilename != NULL) {
    strcpy(fn, (const char *)definedFilename);
    if (strncmp(&fn[strlen(fn)-3], "act", 3) == 0) {
      strcpy(&fn[strlen(fn)-3], "spr");
    }
    def = true;
  }

  GtkWidget *menuAudit = glade_xml_get_widget(windowXML, "menuAudit");
  gtk_widget_set_sensitive(menuAudit, FALSE);

  if (def == true || GetFileName(fn, sizeof(fn), "select *.spr or *.bmp", true, FILTER_SPR | FILTER_BMP | FILTER_NONE)) {
    if (strncmp(&fn[strlen(fn)-3], "spr", 3) != 0
     && strncmp(&fn[strlen(fn)-3], "bmp", 3) != 0)  return;
    actor->drawStop = true;
    delete actor;
    actor = new CActor();
    actor->SetColors(colorBG, colorSelect, colorLine);
    char ttl[1024+50];
    SNPRINTF(ttl, sizeof(ttl), "%s - %s", THIS_APP_NAME, fn);
    if (actor->ReadFileSPRorBMPAndAct(fn)) {
      //AuditAll(NULL, NULL);
      ProcAfterReadSPR_ACT(fn);
      strcat(ttl, " & act");
      gtk_widget_set_sensitive(menuAudit, TRUE);
    }
    GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
    GtkWidget *oglFrmImg = glade_xml_get_widget(windowXML, "oglFrmImg");
    CameraDefault(oglAllImg, &actor->cameraAll, &actor->widthAll, &actor->heightAll, &actor->ratioAll);
    CameraDefault(oglFrmImg, &actor->cameraFrmImg, &actor->widthImage, &actor->heightImage, &actor->ratioFrmImg);
    gtk_window_set_title(GTK_WINDOW(window), ttl);
    UndoRedoInit();
    actor->drawStop = false;
    GtkWidget *scrImage = glade_xml_get_widget(windowXML, "scrImage");
    SetScrollPos_(scrImage, actor->curImg, actor->GetNumImage()-1);
    //SetImages(0);
  }
}

LIBGLADE_SIGNAL_FUNC void WriteAct(GtkMenuItem *menuitem, gpointer user_data)
{
  char fn[FILENAME_MAX];

  if (GetFileName(fn, sizeof(fn), "Save act", false, FILTER_ACT | FILTER_NONE)) {
    actor->CheckIfAddSuffix(fn, sizeof(fn), ".act");
    actor->WriteAct(fn);
  }
}

LIBGLADE_SIGNAL_FUNC void WriteActText(GtkMenuItem *menuitem, gpointer user_data)
{
  char fn[FILENAME_MAX];

  if (GetFileName(fn, sizeof(fn), "Save act as text", false, FILTER_TXT | FILTER_NONE)) {
    actor->CheckIfAddSuffix(fn, sizeof(fn), ".txt");
    actor->WriteActText(fn);
  }
}

LIBGLADE_SIGNAL_FUNC void WriteBmp(GtkMenuItem *menuitem, gpointer user_data)
{
  char fn[FILENAME_MAX];

  if (GetFileName(fn, sizeof(fn), "Save bmp(&tga)", false, FILTER_BMP | FILTER_TGA | FILTER_NONE) == false)  return;
  actor->WriteBmp256Tga32(fn , -1, false);
}

LIBGLADE_SIGNAL_FUNC void WriteSpr(GtkMenuItem *menuitem, gpointer user_data)
{
  char fn[FILENAME_MAX];

  if (GetFileName(fn, sizeof(fn), "Save spr", false, FILTER_SPR | FILTER_NONE)) {
    actor->CheckIfAddSuffix(fn, sizeof(fn), ".spr");
    actor->WriteSpr(fn);
  }
}

LIBGLADE_SIGNAL_FUNC void ReplaceImage(GtkMenuItem *menuitem, gpointer user_data)
{
  if (IsSprReady() == false)  return;

  char fn[FILENAME_MAX];

  if (actor->curImg < actor->GetNumPalImage()) {
    if (GetFileName(fn, sizeof(fn), "Select 256color bitmap file", true, FILTER_BMP | FILTER_NONE)) {
      actor->ReplaceImage(fn);
    }
  } else {
    if (GetFileName(fn, sizeof(fn), "Select 32bit tga file", true, FILTER_TGA | FILTER_NONE)) {
      actor->ReplaceImage(fn);
    }
  }
  SetImages(actor->curImg);
}

LIBGLADE_SIGNAL_FUNC void Write1Image(GtkMenuItem *menuitem, gpointer user_data)
{
  if (IsSprReady() == false)  return;

  char fn[FILENAME_MAX];

  if (actor->curImg < actor->GetNumPalImage()) {
    if (GetFileName(fn, sizeof(fn), "Set 256color bitmap filename", false, FILTER_BMP | FILTER_NONE)) {
      actor->CheckIfAddSuffix(fn, sizeof(fn), ".bmp");
      actor->WriteBmp256Tga32(fn, actor->curImg, true);
    }
  } else {
    if (GetFileName(fn, sizeof(fn), "Set 32bit tga filename", false, FILTER_TGA | FILTER_NONE)) {
      actor->CheckIfAddSuffix(fn, sizeof(fn), ".tga");
      actor->WriteBmp256Tga32(fn, actor->curImg, true);
    }
  }
}

LIBGLADE_SIGNAL_FUNC void InsertImage(GtkWidget *menuitem, gpointer user_data)
{
  if (IsSprReady() == false)  return;

  char fn[FILENAME_MAX];

  int pointInsert;
  if (strcmp(glade_get_widget_name(menuitem), "menuInsertBefore") == 0) {
    pointInsert = actor->curImg;
  } else {
    pointInsert = actor->curImg+1;
  }

  if (GetFileName(fn, sizeof(fn), "Select inserted image", true, FILTER_BMP | FILTER_TGA | FILTER_NONE)) {
    if (strncmp(&fn[strlen(fn)-4], ".bmp", 4) == 0) {
      if (actor->GetNumFlatImage()) {
        if (pointInsert > actor->GetNumPalImage()) {
          GtkWidget *mes;
          mes = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
                  GTK_BUTTONS_OK, _("Can't insert palleted image there."));
          gtk_dialog_run(GTK_DIALOG(mes));
          gtk_widget_destroy(mes);
          return;
        }
      }
    } else if (strncmp(&fn[strlen(fn)-4], ".tga", 4) == 0) {
      if (pointInsert < actor->GetNumPalImage() - 1) {
        GtkWidget *mes;
        mes = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK, _("Can't insert flat image there."));
        gtk_dialog_run(GTK_DIALOG(mes));
        gtk_widget_destroy(mes);
        return;
      }
    } else {
      GtkWidget *mes;
      mes = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
              GTK_BUTTONS_OK, _("Unhandled suffix."));
      gtk_dialog_run(GTK_DIALOG(mes));
      gtk_widget_destroy(mes);
      return;
    }
    actor->InsertImage(pointInsert, fn);
    GtkWidget *scrImage = glade_xml_get_widget(windowXML, "scrImage");
    SetScrollPos_(scrImage, actor->curImg, actor->GetNumImage()-1);
    //SetImages(actor->curImg);
    ProcPatternChange(true);
  }
}

LIBGLADE_SIGNAL_FUNC void DeleteImage(GtkMenuItem *menuitem, gpointer user_data)
{
  if (IsSprReady() == false)  return;

  actor->DeleteCurrentImage();

  GtkWidget *scrImage = glade_xml_get_widget(windowXML, "scrImage");
  SetScrollPos_(scrImage, actor->curImg, actor->GetNumImage()-1);
  //SetImages(actor->curImg);
  ProcPatternChange(true);
}

LIBGLADE_SIGNAL_FUNC void ProcMenuNextFrame(GtkMenuItem *menuitem, gpointer user_data)
{
  AdjustBoundsScrFrame(NULL, actor->curPat+1, NULL);
}

LIBGLADE_SIGNAL_FUNC void ProcMenuPrevFrame(GtkMenuItem *menuitem, gpointer user_data)
{
  AdjustBoundsScrFrame(NULL, actor->curPat-1, NULL);
}

LIBGLADE_SIGNAL_FUNC void SetSameColorUnusedPal(GtkMenuItem *menuitem, gpointer user_data)
{
  if (IsSprReady() == false)  return;

  GtkWidget *csdialog;
  csdialog = gtk_color_selection_dialog_new(NULL);
  if (gtk_dialog_run(GTK_DIALOG(csdialog)) == GTK_RESPONSE_OK) {
    GdkColor color;
    gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csdialog)->colorsel), &color);
    BYTE r, g, b;
    r = color.red >> 8;
    g = color.green >> 8;
    b = color.blue >> 8;
    actor->SetSameColorUnusedPal(b << 16 | g << 8 | r);
  }
  gtk_widget_destroy(csdialog);
}

void SpritePan(BYTE key)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  int *iItems = NULL;
  if (numS) {
    iItems = (int *)malloc(numS * sizeof(int));
    GetSelectedTableItem(iItems);

    actor->SpritePan(key, iItems, numS);

    free(iItems);
  }
  ProcPatternChange(false);
  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  gtk_widget_queue_draw(oglAllImg);
}

void SpriteZoom(bool equalRatio)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkTreeModel *model = gtk_tree_view_get_model(table);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(table);
  GtkTreeIter iter;

  int iItem = 0;
  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return;
  do {
    if (gtk_tree_selection_iter_is_selected(selection, &iter)) break;
    iItem++;
  } while (gtk_tree_model_iter_next(model, &iter));

  actor->SpriteZoom(iItem, equalRatio);
  ProcPatternChange(false);
  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  gtk_widget_queue_draw(oglAllImg);
}

void SpriteRot(void)
{
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  GtkTreeModel *model = gtk_tree_view_get_model(table);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(table);
  GtkTreeIter iter;

  int iItem = 0;
  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)  return;
  do {
    if (gtk_tree_selection_iter_is_selected(selection, &iter)) break;
    iItem++;
  } while (gtk_tree_model_iter_next(model, &iter));

  actor->SpriteRot(iItem);
  ProcPatternChange(false);
  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  gtk_widget_queue_draw(oglAllImg);
}

void CameraPan(void)
{
  actor->CameraPan();
  DrawFrameAll();
}

void CameraZoom(void)
{
  actor->CameraZoom();
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void SelectColor(GtkWidget *menuitem, gpointer user_data)
{
  GtkWidget *csdialog;
  csdialog = gtk_color_selection_dialog_new(NULL);
  BYTE r, g, b;
  GdkColor color;
  int wid;
  if (strcmp(glade_get_widget_name(menuitem), "menuSetColorBG") == 0) {
    wid = 1;
  } else if (strcmp(glade_get_widget_name(menuitem), "menuSetColorLine") == 0) {
    wid = 2;
  } else {
    wid = 3;
  }
  if (wid == 1) {
    r = GETCOL_R(colorBG);
    g = GETCOL_G(colorBG);
    b = GETCOL_B(colorBG);
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csdialog)->colorsel), &color);
  } else if (wid == 2) {
    r = GETCOL_R(colorSelect);
    g = GETCOL_G(colorSelect);
    b = GETCOL_B(colorSelect);
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csdialog)->colorsel), &color);
  } else {
    r = GETCOL_R(colorLine);
    g = GETCOL_G(colorLine);
    b = GETCOL_B(colorLine);
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csdialog)->colorsel), &color);
  }
  if (gtk_dialog_run(GTK_DIALOG(csdialog)) != GTK_RESPONSE_OK) return;

  gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csdialog)->colorsel), &color);
  r = color.red >> 8;
  g = color.green >> 8;
  b = color.blue >> 8;
  if (wid == 1) {
    colorBG = actor->colorBG = b << 16 | g << 8 | r;
    GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
    GtkWidget *oglFrmImg = glade_xml_get_widget(windowXML, "oglFrmImg");
    SetupOpenGL(oglAllImg);
    SetupOpenGL(oglFrmImg);
  } else if (wid == 2) {
    colorSelect = b << 16 | g << 8 | r;
  } else {
    colorLine = b << 16 | g << 8 | r;
  }
  SetImages(actor->curImg);
  DrawFrameAll();
  gtk_widget_destroy(csdialog);
  actor->SetColors(colorBG, colorSelect, colorLine);
}

LIBGLADE_SIGNAL_FUNC void ChangeStateHLine(GtkMenuItem *menuHLine, gpointer user_data)
{
  actor->drawHLine = !actor->drawHLine;
  //gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuHLine), actor->drawHLine);
  (GTK_CHECK_MENU_ITEM(menuHLine))->active = actor->drawHLine;
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void ChangeStateVLine(GtkMenuItem *menuVLine, gpointer user_data)
{
  actor->drawVLine = !actor->drawVLine;
  //gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuVLine), actor->drawVLine);
  (GTK_CHECK_MENU_ITEM(menuVLine))->active = actor->drawVLine;
  DrawFrameAll();
}

LIBGLADE_SIGNAL_FUNC void Undo(GtkMenuItem *menuitem, gpointer user_data)
{
  actor->DoPrevUserCommand();
  if (actor->curRec == actor->startRec){
    UndoEnable(false);
  } else {
    UndoEnable(true);
  }
  RedoEnable(true);

  ProcPatternChange(true);
}

LIBGLADE_SIGNAL_FUNC void Redo(GtkMenuItem *menuitem, gpointer user_data)
{
  actor->DoNextUserCommand();
  if (actor->curRec == actor->endRec) {
    RedoEnable(false);
  } else {
    RedoEnable(true);
  }
  UndoEnable(true);
                                                                                
  ProcPatternChange(true);
}

bool OnClose_Save(bool frommenu)
{
  if (idTimer) {
    gtk_timeout_remove(idTimer);
    idTimer = 0;
  }

  if (IsActReady()) {
    if (actor->GetModified()) {
      GtkWidget *mes;
      gint ret;
      mes = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING,
              GTK_BUTTONS_YES_NO, _("ACT is edited.\n\nDo you want to save ACT?"));
      if (frommenu) {
        gtk_dialog_add_button(GTK_DIALOG(mes), "gtk-cancel", GTK_RESPONSE_CANCEL);
      }
      ret = gtk_dialog_run(GTK_DIALOG(mes));
      gtk_widget_destroy(mes);
      if (ret == GTK_RESPONSE_YES) {
        WriteAct(NULL, NULL);
      } else if (ret == GTK_RESPONSE_CANCEL) {
        return false;
      }
    }
  }
  return true;
}

bool IsSprReady(void)
{
  if (actor == NULL || actor->IsSprReady() == false)  return false;

  return true;
}

bool IsActReady(void)
{
  if (actor == NULL || actor->IsActReady() == false)  return false;

  return true;
}

void ProcAfterReadSPR_ACT(char *fn)
{
  if (IsActReady() == false)  return;

  CheckReference(fn);

  char buf[128];
  GtkOptionMenu *optAct = (GtkOptionMenu *)glade_xml_get_widget(windowXML, "optAct");
  GtkWidget *menu;
  GtkWidget *menui;
  gtk_option_menu_remove_menu(optAct);
  menu = gtk_menu_new();
  int i;
  int numact8 = actor->GetNumAction() / 8;
  if (actor->GetNumAction() % 8)  numact8++;
  for (i=0; i<numact8; i++) {
    SNPRINTF(buf, sizeof(buf), "act%02d", i);
    menui = gtk_menu_item_new_with_label(buf);
    gtk_widget_show(menui);
    gtk_container_add(GTK_CONTAINER(menu), menui);
  }
  gtk_option_menu_set_menu(optAct, menu);
  //act sound
  int numsnd = actor->GetNumSounds();
  menu = gtk_menu_new();
  menui = gtk_menu_item_new_with_label("nothing");
  gtk_widget_show(menui);
  gtk_container_add(GTK_CONTAINER(menu), menui);
  for (int i=0; i<numsnd; i++) {
    SNPRINTF(buf, sizeof(buf), "%s", actor->GetSoundFN(i));
    menui = gtk_menu_item_new_with_label(buf);
    gtk_widget_show(menui);
    gtk_container_add(GTK_CONTAINER(menu), menui);
  }
  GtkOptionMenu *optSound = (GtkOptionMenu *)glade_xml_get_widget(windowXML, "optSound");
  gtk_option_menu_set_menu(optSound, menu);
}

void CheckReference(char *fn)
{
  char d[FILENAME_MAX];
  GETCWD(d, sizeof(d));

  char nd[FILENAME_MAX];
  strncpy(nd, fn, sizeof(nd));
  int i;
  for (i=(int)strlen(fn); i>=0; i--) {
#ifdef WIN32
    if (fn[i] == '\\') break;
#else
    if (fn[i] == '/') break;
#endif
  }
  if (i != 0) {
    nd[i] = '\0';
  } else {
#ifdef WIN32
    strcpy(nd, ".\\");
#else
    strcpy(nd, "./");
#endif
  }

  actor->CheckReferenceAny(d, nd);

  CheckRefHead();
  CheckRefBody();
  CheckRefEtc();
  CheckRefNeigh();

  actor->ChangeNeighborPoint(2);
}

void ProcPatternChange(bool drawAll)
{
  if (IsActReady() == false)  return;

  int cur = actor->curPat;
  int max = actor->GetNumPattern(actor->curAct) - 1;
  // frame all image
  if (drawAll) {
    DrawFrameAll();
  }
  //frame label
  char buf[10];
  SNPRINTF(buf, sizeof(buf), "%d / %d", cur+1, max+1);
  GtkLabel *lblFrame = (GtkLabel *)glade_xml_get_widget(windowXML, "lblFrame");
  gtk_label_set_text(lblFrame, buf);
  //frame scroll
  GtkWidget *scrFrame = glade_xml_get_widget(windowXML, "scrFrame");
  SetScrollPos_(scrFrame, cur, max);
  //table
  SetTableItemAll(actor->curAct, actor->curPat);
  //entry
  SetInterval(actor->GetTimeInterval());
  //combo snd
  bool tmpPlay = autoPlay;
  if (!tmpPlay) {
    autoPlay = true;
  }
  GtkOptionMenu *optSound = (GtkOptionMenu *)glade_xml_get_widget(windowXML, "optSound");
  gtk_option_menu_set_history(optSound, 1 + actor->GetSoundNo(actor->curAct, actor->curPat));
  if (!tmpPlay) {
    autoPlay = tmpPlay;
  }
}

void UndoRedoInit(void)
{
  actor->SetInvalidPrevCommand();
  UndoEnable(false);
  RedoEnable(false);
}

void DispUndoInitWarningFE(void)
{
  if (warningUndoFE) {
    GtkWidget *mes;
    mes = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
      _("If you edit frames, you can't undo.\n\nDoes redisplay this warning?\n\n(Redisplay this warning at next session.)"));
    if (gtk_dialog_run(GTK_DIALOG(mes)) == GTK_RESPONSE_NO) {
      warningUndoFE = false;
    }
  }
}

bool DispUndoInitWarningSPO(void)
{
  if (warningUndoSPO) {
    gint ret;
    GtkWidget *mes;
    mes = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
      _("If you reorder sprites, you can't undo.\n\nDoes redisplay this warning?"));
    gtk_dialog_add_button(GTK_DIALOG(mes), "gtk-cancel", GTK_RESPONSE_CANCEL);
    actor->dragState = 0;
    ret = gtk_dialog_run(GTK_DIALOG(mes));
    gtk_widget_destroy(mes);
    if (ret == GTK_RESPONSE_CANCEL) {
      return false;
    }
    if (ret == GTK_RESPONSE_NO) {
      warningUndoSPO = false;
    }
    return true;
  }
  return true;
}

void UndoEnable(bool enable)
{
  GtkWidget *menuUndo = glade_xml_get_widget(windowXML, "menuUndo");
  gtk_widget_set_sensitive(menuUndo, (gboolean)enable);
}
                                                                                               
void RedoEnable(bool enable)
{
  GtkWidget *menuRedo = glade_xml_get_widget(windowXML, "menuRedo");
  gtk_widget_set_sensitive(menuRedo, (gboolean)enable);
}

void BeginSpritePan(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_MOVE_XY)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_MOVE_EXY) ) {
    StoreSpritePan();
  }
}

void EndSpritePan(void)
{
  StoreSpritePan();
  UndoEnable(true);
}

void StoreSpritePan(void)
{
  USERCOMMAND tmpcmd;
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems);

  int xOffs, yOffs;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      tmpcmd.type = COMTYPE_MOVE_EXY;
      actor->GetExtOffsPoint(&xOffs, &yOffs, actor->curAct, actor->curPat);
      tmpcmd.xyi[0] = xOffs;
      tmpcmd.xyi[1] = yOffs;
    } else {
      tmpcmd.type = COMTYPE_MOVE_XY;
      tmpcmd.sprId = *(iItems+i);
      actor->GetOffsPoint(&xOffs, &yOffs, actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyi[0] = xOffs;
      tmpcmd.xyi[1] = yOffs;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

void BeginSpriteMag(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_MAG_XY) ) {
    StoreSpriteMag();
  }
}

void EndSpriteMag(void)
{
  StoreSpriteMag();
  UndoEnable(true);
}

void StoreSpriteMag(void)
{
  USERCOMMAND tmpcmd;
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems);

  float xMag, yMag;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      break;
    } else {
      tmpcmd.type = COMTYPE_MAG_XY;
      tmpcmd.sprId = *(iItems+i);
      xMag = actor->GetXMagValue(actor->curAct, actor->curPat, *(iItems+i));
      yMag = actor->GetYMagValue(actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyf[0] = xMag;
      tmpcmd.xyf[1] = yMag;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

void BeginSpriteRot(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_ROT) ) {
    StoreSpriteRot();
  }
}

void EndSpriteRot(void)
{
  StoreSpriteRot();
  UndoEnable(true);
}

void StoreSpriteRot(void)
{
  USERCOMMAND tmpcmd;
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems);

  DWORD rot;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      break;
    } else {
      tmpcmd.type = COMTYPE_ROT;
      tmpcmd.sprId = *(iItems+i);
      rot = actor->GetRotValue(actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyd[0] = rot;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

void BeginSpriteMirror(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_MIRROR) ) {
    StoreSpriteMirror();
  }
}

void EndSpriteMirror(void)
{
  StoreSpriteMirror();
  UndoEnable(true);
}

void StoreSpriteMirror(void)
{
  USERCOMMAND tmpcmd;
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems);

  DWORD mir;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      break;
    } else {
      tmpcmd.type = COMTYPE_MIRROR;
      tmpcmd.sprId = *(iItems+i);
      mir = actor->GetMirrorValue(actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyd[0] = mir;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}

void BeginSpriteSaturation(void)
{
  if ( (actor->startRec == actor->curRec)
    || (actor->usrcmd[actor->curRec].type != COMTYPE_SATURATION) ) {
    StoreSpriteSaturation();
  }
}

void EndSpriteSaturation(void)
{
  StoreSpriteSaturation();
  UndoEnable(true);
}

void StoreSpriteSaturation(void)
{
  USERCOMMAND tmpcmd;
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  int *iItems = (int *)malloc(sizeof(int) * numS);
  GetSelectedTableItem(iItems);
                                                                                
  DWORD abgr;
  for (int i=0; i<numS; i++) {
    tmpcmd.actNo = actor->curAct;
    tmpcmd.patNo = actor->curPat;
    if (*(iItems+i) == actor->GetNumSprites(actor->curAct, actor->curPat)) {
      break;
    } else {
      tmpcmd.type = COMTYPE_SATURATION;
      tmpcmd.sprId = *(iItems+i);
      abgr = actor->GetABGRValue(actor->curAct, actor->curPat, *(iItems+i));
      tmpcmd.xyd[0] = abgr;
    }
    actor->StoreUserCommand( &tmpcmd );
  }
  free(iItems);
}


bool GetFileName(char *fn, int fnlen, char *title, bool open, int filter)
{
  GtkWidget *fc;
  if (open) {
    fc = gtk_file_chooser_dialog_new(title, GTK_WINDOW(window), 
           GTK_FILE_CHOOSER_ACTION_OPEN,
           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
           GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  } else {
    fc = gtk_file_chooser_dialog_new(title, GTK_WINDOW(window), 
           GTK_FILE_CHOOSER_ACTION_SAVE,
           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
           GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
  }

  if (filter & FILTER_ACT) {
    GtkFileFilter *filterACT;
    filterACT = gtk_file_filter_new();
    gtk_file_filter_set_name(filterACT, "act file");
    gtk_file_filter_add_pattern(filterACT, "*.act");
    gtk_file_filter_add_pattern(filterACT, "*.ACT");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc), filterACT);
  }
  if (filter & FILTER_SPR) {
    GtkFileFilter *filterSPR;
    filterSPR = gtk_file_filter_new();
    gtk_file_filter_set_name(filterSPR, "spr file");
    gtk_file_filter_add_pattern(filterSPR, "*.spr");
    gtk_file_filter_add_pattern(filterSPR, "*.SPR");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc), filterSPR);
  }
  if (filter & FILTER_BMP) {
    GtkFileFilter *filterBMP;
    filterBMP = gtk_file_filter_new();
    gtk_file_filter_set_name(filterBMP, "bmp file");
    gtk_file_filter_add_pattern(filterBMP, "*.bmp");
    gtk_file_filter_add_pattern(filterBMP, "*.BMP");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc), filterBMP);
  }
  if (filter & FILTER_TGA) {
    GtkFileFilter *filterTGA;
    filterTGA = gtk_file_filter_new();
    gtk_file_filter_set_name(filterTGA, "tga file");
    gtk_file_filter_add_pattern(filterTGA, "*.tga");
    gtk_file_filter_add_pattern(filterTGA, "*.TGA");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc), filterTGA);
  }
  if (filter & FILTER_TXT) {
    GtkFileFilter *filterTXT;
    filterTXT = gtk_file_filter_new();
    gtk_file_filter_set_name(filterTXT, "txt file");
    gtk_file_filter_add_pattern(filterTXT, "*.txt");
    gtk_file_filter_add_pattern(filterTXT, "*.TXT");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc), filterTXT);
  }
  if (filter & FILTER_WAV) {
    GtkFileFilter *filterWAV;
    filterWAV = gtk_file_filter_new();
    gtk_file_filter_set_name(filterWAV, "wav file");
    gtk_file_filter_add_pattern(filterWAV, "*.wav");
    gtk_file_filter_add_pattern(filterWAV, "*.WAV");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc), filterWAV);
  }
  if (filter & FILTER_NONE) {
#ifndef WIN32
/*
#endif
    GtkFileFilter *filterNONE;
    filterNONE = gtk_file_filter_new();
    gtk_file_filter_set_name(filterNONE, "all file");
    gtk_file_filter_add_pattern(filterNONE, "*.*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fc), filterNONE);
#ifndef WIN32
*/
#endif
  }

  strcpy(fn, "");
  bool ret;
  if (gtk_dialog_run(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT) {
    strncpy(fn, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc)), fnlen);
    ret = true;
  } else {
    ret = false;
  }
  gtk_widget_destroy(fc);

  return ret;
}

void SetMousePoint(GdkEventButton *event)
{
  actor->firstMP.x = actor->oldMP.x = (int)event->x;
  actor->firstMP.y = actor->oldMP.y = (int)event->y;
}


LIBGLADE_SIGNAL_FUNC void CloseMainWindow(GtkWidget *widget, gpointer data)
{
  OnClose_Save(false);

  gtk_main_quit();

  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  gtk_widget_unrealize(oglAllImg);
  GtkWidget *oglFrmImg = glade_xml_get_widget(windowXML, "oglFrmImg");
  gtk_widget_unrealize(oglFrmImg);

}

LIBGLADE_SIGNAL_FUNC void CloseWindowFromMenu(GtkMenuItem *menuitem, gpointer user_data)
{
  if (OnClose_Save(true) == false)  return;
  gtk_main_quit();
}

LIBGLADE_SIGNAL_FUNC gboolean KeyReleaseWindow(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  switch (event->keyval) {
  case GDK_Escape:
    UnSelectSelection();
    break;
  case GDK_Left:
    if (altKeyPressed) {
      BeginSpritePan();
      SpritePan(VK_LEFT);
      EndSpritePan();
    }
    break;
  case GDK_Up:
    if (altKeyPressed) {
      BeginSpritePan();
      SpritePan(VK_UP);
      EndSpritePan();
    }
    break;
  case GDK_Right:
    if (altKeyPressed) {
      BeginSpritePan();
      SpritePan(VK_RIGHT);
      EndSpritePan();
    }
    break;
  case GDK_Down:
    if (altKeyPressed) {
      BeginSpritePan();
      SpritePan(VK_DOWN);
      EndSpritePan();
    }
    break;
  case GDK_Delete:
    if (altKeyPressed) {
      if (DispUndoInitWarningSPO() == false)  return TRUE;
      UndoRedoInit();
      DeleteSelectedSpriteInTable();
    }
    break;
  }
  return FALSE;
}

gboolean TimerFrameAll(gpointer data)
{
  actor->curPat++;
  int max = actor->GetNumPattern(actor->curAct);
  if (actor->curPat == max) {
    actor->curPat = 0;
  }
  ProcPatternChange(true);
  return TRUE;
}

void ShowPopupMenu(GdkEventButton *event)
{
  GtkWidget *menu;
  
  menu = gtk_menu_new();
  
  GtkWidget *menui;
  menui = gtk_menu_item_new_with_label(_("Replace image..."));
  gtk_widget_show(menui);
  gtk_signal_connect(GTK_OBJECT(menui), "activate", GTK_SIGNAL_FUNC(ReplaceImage), NULL);
  gtk_container_add(GTK_CONTAINER(menu), menui);

  menui = gtk_menu_item_new_with_label(_("Write one image..."));
  gtk_widget_show(menui);
  gtk_signal_connect(GTK_OBJECT(menui), "activate", GTK_SIGNAL_FUNC(Write1Image), NULL);
  gtk_container_add(GTK_CONTAINER(menu), menui);
  
  menui = gtk_separator_menu_item_new();
  gtk_widget_show(menui);
  gtk_widget_set_sensitive(menui, FALSE);
  gtk_container_add(GTK_CONTAINER(menu), menui);
  
  menui = gtk_menu_item_new_with_label(_("Insert image before..."));
  gtk_widget_show(menui);
  gtk_signal_connect(GTK_OBJECT(menui), "activate", GTK_SIGNAL_FUNC(InsertImage), (gpointer)true);
  gtk_container_add(GTK_CONTAINER(menu), menui);
  
  menui = gtk_menu_item_new_with_label(_("Insert image after..."));
  gtk_widget_show(menui);
  gtk_signal_connect(GTK_OBJECT(menui), "activate", GTK_SIGNAL_FUNC(InsertImage), (gpointer)false);
  gtk_container_add(GTK_CONTAINER(menu), menui);

  menui = gtk_separator_menu_item_new();
  gtk_widget_show(menui);
  gtk_widget_set_sensitive(menui, FALSE);
  gtk_container_add(GTK_CONTAINER(menu), menui);
  
  menui = gtk_menu_item_new_with_label(_("Delete image"));
  gtk_widget_show(menui);
  gtk_signal_connect(GTK_OBJECT(menui), "activate", GTK_SIGNAL_FUNC(DeleteImage), NULL);
  gtk_container_add(GTK_CONTAINER(menu), menui);
  
  menui = gtk_separator_menu_item_new();
  gtk_widget_show(menui);
  gtk_widget_set_sensitive(menui, FALSE);
  gtk_container_add(GTK_CONTAINER(menu), menui);
  
  menui = gtk_menu_item_new_with_label(_("Set same color empty pallet"));
  gtk_widget_show(menui);
  gtk_signal_connect(GTK_OBJECT(menui), "activate", GTK_SIGNAL_FUNC(SetSameColorUnusedPal), NULL);
  gtk_container_add(GTK_CONTAINER(menu), menui);

  gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button, event->time);
}

LIBGLADE_SIGNAL_FUNC gboolean configureOGL(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
                                                                                
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }
  SetupOpenGL(widget);
  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  if (widget == oglAllImg) {
    CameraDefault(widget, &actor->cameraAll, &actor->widthAll, &actor->heightAll, &actor->ratioAll);
  } else {
    CameraDefault(widget, &actor->cameraFrmImg, &actor->widthImage, &actor->heightImage, &actor->ratioFrmImg);
  }
  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers (gldrawable);
  } else {
    glFlush();
  }
  gdk_gl_drawable_gl_end(gldrawable);
                                                                                
  return TRUE;
}

LIBGLADE_SIGNAL_FUNC gboolean exposeAllImg(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
 
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }

  DrawFrameAll();

  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers (gldrawable);
  } else {
    glFlush();
  }
                                                                                                
  gdk_gl_drawable_gl_end(gldrawable);
                                                                                                
  return TRUE;
}

LIBGLADE_SIGNAL_FUNC gboolean exposeFrmImg(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }

  SetImages(actor->curImg); 

  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers (gldrawable);   } else {
    glFlush();
  }
                                                                                                
  gdk_gl_drawable_gl_end(gldrawable);
                                                                                                
  return TRUE;
}

LIBGLADE_SIGNAL_FUNC gboolean motionAllImg(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  actor->newMP.x = (int)event->x;
  actor->newMP.y = (int)event->y;
  switch (actor->dragState) {
  case DRAG_ALL_S_PAN:
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return TRUE;
    SpritePan(0);
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return TRUE;
  case DRAG_ALL_S_ZOOM:
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return TRUE;
    if (shiftKeyPressed) {
      SpriteZoom(true);
    } else {
      SpriteZoom(false);
    }
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return TRUE;
  case DRAG_ALL_S_ROT:
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return TRUE;
    SpriteRot();
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return TRUE;
  case DRAG_ALL_C_PAN:
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return TRUE;
    CameraPan();
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return TRUE;
  case DRAG_ALL_C_ZOOM:
    if ((actor->newMP.x == actor->oldMP.x)
      && (actor->newMP.y == actor->oldMP.y))  return TRUE;
    CameraZoom();
    actor->oldMP.x = actor->newMP.x;
    actor->oldMP.y = actor->newMP.y;
    return TRUE;
  }
  return TRUE;
}

LIBGLADE_SIGNAL_FUNC gboolean bpressAllImg(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  SetMousePoint(event);
  GtkTreeView *table = (GtkTreeView *)glade_xml_get_widget(windowXML, "table");
  int numS = gtk_tree_selection_count_selected_rows(gtk_tree_view_get_selection(table));
  if (numS == 0 || ctrlKeyPressed) {  // camera
    switch (event->button) {
    case 1:  // left
    case 4:
      if (mouseAssign.pan == MK_LBUTTON) {
        actor->dragState = DRAG_ALL_C_PAN;
      } else if (mouseAssign.zoom == MK_LBUTTON) {
        actor->dragState = DRAG_ALL_C_ZOOM;
      }
      break;
    case 2:  // middle
      if (mouseAssign.pan == MK_MBUTTON) {
        actor->dragState = DRAG_ALL_C_PAN;
      } else if (mouseAssign.zoom == MK_MBUTTON) {
        actor->dragState = DRAG_ALL_C_ZOOM;
      }
      break;
    case 3:  // right
    case 5:
      if (mouseAssign.pan == MK_RBUTTON) {
        actor->dragState = DRAG_ALL_C_PAN;
      } else if (mouseAssign.zoom == MK_RBUTTON) {
        actor->dragState = DRAG_ALL_C_ZOOM;
      }
      break;
    }
  } else {  // sprite
    switch (event->button) {
    case 1:  // left
    case 4:
      if (mouseAssign.pan == MK_LBUTTON) {
        actor->dragState = DRAG_ALL_S_PAN;
        BeginSpritePan();
      } else if (mouseAssign.zoom == MK_LBUTTON) {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ZOOM;
          BeginSpriteMag();
        }
      } else {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ROT;
          BeginSpriteRot();
        }
      }
      break;
    case 2:  // middle
      if (mouseAssign.pan == MK_MBUTTON) {
        actor->dragState = DRAG_ALL_S_PAN;
        BeginSpritePan();
      } else if (mouseAssign.zoom == MK_MBUTTON) {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ZOOM;
          BeginSpriteMag();
        }
      } else {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ROT;
          BeginSpriteRot();
        }
      }
      break;
    case 3:  // right
    case 5:
      if (mouseAssign.pan == MK_RBUTTON) {
        actor->dragState = DRAG_ALL_S_PAN;
        BeginSpritePan();
      } else if (mouseAssign.zoom == MK_RBUTTON) {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ZOOM;
          BeginSpriteMag();
        }
      } else {
        if (numS == 1) {
          actor->dragState = DRAG_ALL_S_ROT;
          BeginSpriteRot();
        }
      }
      break;
    }
    ProcPatternChange(true);
  }
  return TRUE;
}

LIBGLADE_SIGNAL_FUNC gboolean breleaseAllImg(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  switch (actor->dragState) {
  case DRAG_ALL_S_PAN:
    actor->dragState = 0;
    ProcPatternChange(true);
    EndSpritePan();
    break;
  case DRAG_ALL_S_ZOOM:
    actor->dragState = 0;
    ProcPatternChange(true);
    EndSpriteMag();
    break;
  case DRAG_ALL_S_ROT:
    actor->dragState = 0;
    ProcPatternChange(true);
    EndSpriteRot();
    break;
  case DRAG_ALL_C_PAN:
  case DRAG_ALL_C_ZOOM:
    actor->dragState = 0;
    ProcPatternChange(true);
    break;
  }
  return TRUE;
}

LIBGLADE_SIGNAL_FUNC gboolean keypressAllImg(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  if (event->keyval == GDK_Control_L || event->keyval == GDK_Control_R) {
    ctrlKeyPressed = true;
  }
  if (event->keyval == GDK_Shift_L || event->keyval == GDK_Shift_R) {
    shiftKeyPressed = true;
  }
  if (event->keyval == GDK_Alt_L || event->keyval == GDK_Alt_R) {
    altKeyPressed = true;
  }
  return FALSE;
}

LIBGLADE_SIGNAL_FUNC gboolean keyreleaseAllImg(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  if (event->keyval == GDK_Control_L || event->keyval == GDK_Control_R) {
    ctrlKeyPressed = false;
  }
  if (event->keyval == GDK_Shift_L || event->keyval == GDK_Shift_R) {
    shiftKeyPressed = false;
  }
  if (event->keyval == GDK_Alt_L || event->keyval == GDK_Alt_R) {
    altKeyPressed = false;
  }
  return FALSE;
}

LIBGLADE_SIGNAL_FUNC gboolean bpressFrmImg(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if (event->button == 3) {
    ShowPopupMenu(event);
    return TRUE;
  }
  actor->dragState = DRAG_IMAGE;
  SetMousePoint(event);
  int newno = GetClickImgNo((int)event->x);
  if (newno == -1) {
    actor->firstMP.x = 0;
    return TRUE;
  } else {
    actor->firstMP.x = newno;
  }
  SetImages(newno);
  return TRUE;
}

LIBGLADE_SIGNAL_FUNC gboolean breleaseFrmImg(GtkWidget *oglFrmImg, GdkEventButton *event, gpointer data)
{
  if (actor->dragState == DRAG_IMAGE) {
    if (IsActReady()) {
      int x, y, ox, oy, ox2, oy2, ow, oh, ow2, oh2;
      x = (int)event->x + oglFrmImg->allocation.x;
      y = (int)event->y + oglFrmImg->allocation.y;
      GtkWidget *table = glade_xml_get_widget(windowXML, "table");
      ox = (int)table->allocation.x;
      oy = (int)table->allocation.y;
      ow = (int)table->allocation.width;
      oh = (int)table->allocation.height;
      GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "table");
      ox2 = (int)oglAllImg->allocation.x;
      oy2 = (int)oglAllImg->allocation.y;
      ow2 = (int)oglAllImg->allocation.width;
      oh2 = (int)oglAllImg->allocation.height;
      if ((ox <= x && x <= ox+ow && oy <= y && y <= oy+oh) 
       || (ox2 <= x && x <= ox2+ow2 && oy2 <= y && y <= oy2+oh2)) {
        if (DispUndoInitWarningSPO()) {
          actor->AddSpr(actor->curAct, actor->curPat, actor->firstMP.x);
        }
      }
    }
  }

  actor->dragState = 0;
  GtkWidget *scrImage = glade_xml_get_widget(windowXML, "scrImage");
  if (IsSprReady()) {
    SetScrollPos_(scrImage, actor->curImg, actor->GetNumImage()-1);
  } else {
    SetScrollPos_(scrImage, actor->curImg, 0);
  }
  actor->curImg = actor->firstMP.x;
  SetImages(actor->curImg);
  ProcPatternChange(true);
  return TRUE;
}

void SignalConnect(void)
{
  glade_xml_signal_autoconnect(windowXML);
  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  GtkWidget *oglFrmImg = glade_xml_get_widget(windowXML, "oglFrmImg");
  gtk_widget_add_events(oglAllImg, GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK | GDK_BUTTON3_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_VISIBILITY_NOTIFY_MASK);
  gtk_widget_add_events(oglFrmImg, GDK_BUTTON1_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_VISIBILITY_NOTIFY_MASK);
  GtkWidget *menuInsertBefore = glade_xml_get_widget(windowXML, "menuInsertBefore");
  GtkWidget *menuInsertAfter = glade_xml_get_widget(windowXML, "menuInsertAfter");
}

void BuildWindow(void)
{
  //windowXML = glade_xml_new("D:\\home\\ArcData\\actor\\actor.glade", NULL, NULL);

  windowXMLstring = (unsigned char *)malloc(WINDOWXMLSTRING_ORIG_LEN);
  uLong realdestlen;
  uncompress((Bytef *)windowXMLstring, &realdestlen, (const Bytef *)windowXMLstringcomp, WINDOWXMLSTRING_COMP_LEN);
  windowXML = glade_xml_new_from_buffer((const char *)windowXMLstring, WINDOWXMLSTRING_ORIG_LEN, NULL, NULL);
  free(windowXMLstring);

  window = glade_xml_get_widget(windowXML, "window");

  GtkWidget *oglAllImg = glade_xml_get_widget(windowXML, "oglAllImg");
  gtk_widget_set_gl_capability(oglAllImg, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);
  gtk_widget_realize(oglAllImg);
  GtkWidget *oglFrmImg = glade_xml_get_widget(windowXML, "oglFrmImg");
  gtk_widget_set_gl_capability(oglFrmImg, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);
  gtk_widget_realize(oglFrmImg);


  GtkWidget *table;
  GtkListStore *store;
  table = glade_xml_get_widget(windowXML, "table");
  store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
  gtk_tree_view_set_model(GTK_TREE_VIEW(table), GTK_TREE_MODEL(store));
  BuildTable(table, store);
}

void BuildTable(GtkWidget *table, GtkListStore *store)
{
  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)SPRNO_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "SprNo", renderer,
    "text", SPRNO_COLUMN, "editable", EDITABLE_COLUMN, NULL);
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)SPTYPE_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "SpType", renderer,
    "text", SPTYPE_COLUMN, "editable", EDITABLE_COLUMN, NULL);
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)XOFFS_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "Xoffs", renderer,
    "text", XOFFS_COLUMN, "editable", EDITABLE_COLUMN, NULL);
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)YOFFS_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "Yoffs", renderer,
    "text", YOFFS_COLUMN, "editable", EDITABLE_COLUMN, NULL);
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)MIRROR_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "Mirror", renderer,
    "text", MIRROR_COLUMN, "editable", EDITABLE_COLUMN, NULL);
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)SAT_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "AABBGGRR", renderer,
    "text", SAT_COLUMN, "editable", EDITABLE_COLUMN, NULL);
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)XMAG_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "Xmag", renderer,
    "text", XMAG_COLUMN, "editable", EDITABLE_COLUMN, NULL);
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)YMAG_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "Ymag", renderer,
    "text", YMAG_COLUMN, "editable", EDITABLE_COLUMN, NULL);
  renderer = gtk_cell_renderer_text_new();
  g_signal_connect(renderer, "edited", G_CALLBACK(EditedTable), store);
  g_object_set_data(G_OBJECT(renderer), "column", (gint *)ROT_COLUMN);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, "Rot", renderer,
    "text", ROT_COLUMN, "editable", EDITABLE_COLUMN, NULL);
                                                                                
  gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(table), TRUE);
  gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(table)),    GTK_SELECTION_MULTIPLE);
  gtk_widget_set_size_request(table, 200, 200);
}

void SetTextDomain(char *progname)
{
  setlocale(LC_ALL, getenv("LANG"));
#ifndef DISTRIBUTE
  strcpy(NativeLANG, locale_charset());
#endif
                                                                                
#if WIN32
  set_program_name(progname);
#endif
                                                                                
  char *ddir;
                                                                                
  ddir = getenv("TEXTDOMAINDIR");
  if ((ddir = getenv("TEXTDOMAINDIR")) != NULL) {
  } else {
#if WIN32
    bindtextdomain(MYTEXTDOMAIN, relocate(".\\share\\locale\\"));
#else
    bindtextdomain(MYTEXTDOMAIN, "/usr/local/share/locale");
#endif
  }
  textdomain(MYTEXTDOMAIN);
                                                                                
#ifdef WIN32
#ifdef USE_GTK
#ifdef DISTRIBUTE
  bind_textdomain_codeset(MYTEXTDOMAIN, "UTF-8");
#endif
#endif
#endif
                                                                                
#ifndef WIN32
#ifdef DISTRIBUTE
  bind_textdomain_codeset(MYTEXTDOMAIN, "UTF-8");
#endif
#endif
                                                                                
}

bool GetAgreement(void)
{
  agreementstring = (char *)malloc(AGREEMENTSTRING_ORIG_LEN+1);
  uLong realdestlen;
  uncompress((Bytef *)agreementstring, &realdestlen,
             (const Bytef *)agreementstringcomp, AGREEMENTSTRING_COMP_LEN);
  agreementstring[AGREEMENTSTRING_ORIG_LEN] = '\0';

  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
             GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, 
           _(agreementstring));
  free(agreementstring);
  
  bool ret;
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
    ret = true;
  } else {
    ret = false;
  }

  gtk_widget_destroy(dialog);
  return ret;
}

void GetPrefFileName(char *fname, int fname_size)
{
  char *env_home = getenv("HOME");
  if (env_home) {
    strcpy(fname, env_home);
#ifdef WIN32
    strcat(fname, "\\");
#else
    strcat(fname, "/");
#endif
    strcat(fname, "."THIS_APP_NAME".pref");
  } else {
#ifdef WIN32
    GetModuleFileName(NULL, fname, fname_size);
    for (int i=(int)strlen(fname); i >= 0; i--) {
      if (fname[i] == '\\') {
        fname[i] = 0;
        break;
      }
    }
    strcat(fname, "\\."THIS_APP_NAME".pref");
#else
    strcpy(fname, "."THIS_APP_NAME".pref");
#endif
  }
}

void ReadPreference()
{
  agreement = false;
  mouseAssign.pan = MK_LBUTTON;
  mouseAssign.zoom = MK_MBUTTON;
  mouseAssign.rot = MK_RBUTTON;
  colorBG = 0;
  colorSelect = 0xFF;
  colorLine = 0xFF;

  char fname[FILENAME_MAX];
  GetPrefFileName(fname, sizeof(fname));

  FILE *fp;
  if ((fp = fopen(fname, "r")) == NULL) return;

  char buf[256];
  // 1st line is agreement
  fgets(buf, sizeof(buf), fp);
  atoi(buf) ? agreement = true : agreement = false;
  // 2nd line is panning button
  fgets(buf, sizeof(buf), fp);
  mouseAssign.pan = atoi(buf);
  // 3rd line is zooming button
  fgets(buf, sizeof(buf), fp);
  mouseAssign.zoom = atoi(buf);
  // 4th line is rotation button
  fgets(buf, sizeof(buf), fp);
  mouseAssign.rot = atoi(buf);
  // 5th line is color BG
  fgets(buf, sizeof(buf), fp);
  colorBG = atoi(buf);
  // 6th line is color Selection
  fgets(buf, sizeof(buf), fp);
  colorSelect = atoi(buf);
  // 7th line is color indicator line
  fgets(buf, sizeof(buf), fp);
  colorLine = atoi(buf);

  fclose(fp);
}

void WritePreference()
{
  char fname[FILENAME_MAX];
  GetPrefFileName(fname, sizeof(fname));

  FILE *fp;
  if ((fp = fopen(fname, "w")) == NULL) return;

  // 1st line is agreement
  if (agreement) {
    fprintf(fp, "1\n");
  } else {
    fprintf(fp, "0\n");
  }
  // 2nd line is panning button
  fprintf(fp, "%d\n", mouseAssign.pan);
  // 3rd line is zooming button
  fprintf(fp, "%d\n", mouseAssign.zoom);
  // 4th line is rotation button
  fprintf(fp, "%d\n", mouseAssign.rot);
  // 5th line is color BG
  fprintf(fp, "%d\n", colorBG);
  // 6th line is color Selection
  fprintf(fp, "%d\n", colorSelect);
  // 7th line is color indicator line
  fprintf(fp, "%d\n", colorLine);

  fclose(fp);
}
