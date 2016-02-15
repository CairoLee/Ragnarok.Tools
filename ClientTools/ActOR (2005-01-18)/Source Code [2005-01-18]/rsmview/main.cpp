#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include "../common/relocate/progname.h"
#include "../common/relocate/relocatable.h"
#endif

#include <gdk/gdkkeysyms.h>

#include "main.h"
#include "../actor/gtk/mbutton_assign.h"

#define THIS_APP_NAME "rsmview"

extern "C" {
  extern 
#ifdef WIN32
  __declspec(dllimport)
#endif
  const char *locale_charset(void);
}
char NativeLANG[20];

CRsmview *rsmview;
CPAK *pak1;

MOUSE_ASSIGN mouseAssign;
bool shiftState;

GtkWidget *window;
GtkAccelGroup *accel_group;
GtkWidget *oglarea;
GtkWidget *treeview;
GtkTreeStore *treemodel;

GdkGLConfig *glconfig;

GtkWidget *menuExit;
GtkWidget *menuChangeBGCol;
GtkWidget *menuChangeLineCol;
GtkWidget *menuViewReset;
GtkWidget *menuDispMode;
GtkWidget *menuMAssign;

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("Usage: %s data.{grf|pak} [sdata.{grf|pak} ...]\n", argv[0]);
    return -1;
  }

  SetTextDomain(argv[0]);

  gtk_init(&argc, &argv);
  gtk_gl_init(&argc, &argv);
  glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_ALPHA | GDK_GL_MODE_DOUBLE));
  if (glconfig == NULL) {
      g_print("failed: OpenGL alpha blend\n");
      glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE));
    if (glconfig == NULL) {
      g_print("failed: OpenGL double buffer\n");
      glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)(GDK_GL_MODE_RGBA));    }
  }

  char fnbuf[FILENAME_MAX];
  pak1 = new CPAK();
  for (int i=1; i<argc; i++) {
#ifdef WIN32
    GetFullPathName(argv[i], sizeof(fnbuf), fnbuf, NULL);
#else
    if (argv[i][0] != '/') {
      char fnbuf2[FILENAME_MAX];
      GETCWD(fnbuf2, sizeof(fnbuf2));
      SNPRINTF(fnbuf, sizeof(fnbuf), "%s/%s", fnbuf2, argv[i]);
    } else {
      strcpy(fnbuf, argv[i]);
    }
#endif
    pak1->AddPakFile(fnbuf);
  }
  rsmview = new CRsmview(pak1);

  BuildWindow();
  SignalConnect();
  gtk_widget_show_all(window);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  mouseAssign.pan = MK_RBUTTON;
  mouseAssign.zoom = MK_MBUTTON;
  mouseAssign.rot = MK_LBUTTON;

  gtk_main();

  return 0;
}

void ModelSelect(GtkTreeView *tview, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel *mod;

  if (FindTreeIter(&iter, &mod) == false)  return;
  if (gtk_tree_model_iter_has_child(mod, &iter))  return;

  char fname[FILENAME_MAX];
  MakeOrigFilename(fname, iter, mod);
  if (fname[0] != '\0') {
    rsmview->OpenRSM(fname);
    expose_event(oglarea, NULL, NULL);
  }
}

bool FindTreeIter(GtkTreeIter *iter, GtkTreeModel **mod)
{
  GtkTreeSelection *selection;
                                                                                
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  if (!gtk_tree_selection_get_selected(selection, mod, iter))  return false;
                                                                                
  return true;
}

void MakeOrigFilename(char *fname, GtkTreeIter iter, GtkTreeModel *mod)
{
  GtkTreeIter parent;
  char buf[FILENAME_MAX], buf2[FILENAME_MAX];
  char *name = NULL;
                                                                                
  gtk_tree_model_get(mod, &iter, 1, &name, -1);
  strcpy(buf, name);
  while (gtk_tree_model_iter_parent(mod, &parent, &iter)) {
    gtk_tree_model_get(mod, &parent, 1, &name, -1);
    strcpy(buf2, name);
    if ((pak1->pakversion[0] & 0xFF00) == 0xFF00) {
      sprintf(fname, "%s/%s", buf2, buf);
    } else {
      sprintf(fname, "%s\\%s", buf2, buf);
    }
    strcpy(buf, fname);
    iter = parent;
  }
}

gint CloseMainWindow(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  gtk_main_quit();
                                                                                
  return TRUE;
}

gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
                                                                                
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }
  rsmview->oglwidth = oglarea->allocation.width;
  rsmview->oglheight = oglarea->allocation.height;
  rsmview->InitOpenGL();
  rsmview->SetCameraDefault(false);
  rsmview->Draw();
  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers (gldrawable);
  } else {
    glFlush();
  }
  gdk_gl_drawable_gl_end(gldrawable);
                                                                                
  return TRUE;
}

gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
                                                                                
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }
                                                                                
  rsmview->Draw();
                                                                                
  if (gdk_gl_drawable_is_double_buffered(gldrawable)) {
    gdk_gl_drawable_swap_buffers (gldrawable);
  } else {
    glFlush();
  }
                                                                                
  gdk_gl_drawable_gl_end (gldrawable);
                                                                                
  return TRUE;
}

gboolean motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  rsmview->new_mp.x = (int)event->x;
  rsmview->new_mp.y = (int)event->y;

  switch (rsmview->opState) {
  case NOSTATE:
    break;
  case OBJ_ROT:
    rsmview->GLOBJRot();
    expose_event(widget, NULL, NULL);
    break;
  case SCRN_ROT:
    rsmview->GLSCRNRot();
    expose_event(widget, NULL, NULL);
    break;
  case ZOOM:
    rsmview->GLZoom();
    expose_event(widget, NULL, NULL);
    break;
  case PAN:
    rsmview->GLPan();
    expose_event(widget, NULL, NULL);
    break;
  }

  rsmview->old_mp.x = rsmview->new_mp.x;
  rsmview->old_mp.y = rsmview->new_mp.y;

  return TRUE;
}

gboolean button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  rsmview->first_mp.x = rsmview->old_mp.x = (int)event->x;
  rsmview->first_mp.y = rsmview->old_mp.y = (int)event->y;

  rsmview->CalcCenter();

  int testval;
  switch (event->button) {
  case 1: // left
    testval = MK_LBUTTON;
    break;
  case 2: // middle
    testval = MK_MBUTTON;
    break;
  case 3: // right
    testval = MK_RBUTTON;
    break;
  }
  if (mouseAssign.rot == testval) {
    if (shiftState) {
      rsmview->opState = SCRN_ROT;
    } else {
      rsmview->opState = OBJ_ROT;
    }
    rsmview->SearchRotOrigin();
  } else if (mouseAssign.zoom == testval) {
    rsmview->opState = ZOOM;
  } else if (mouseAssign.pan == testval) {
    rsmview->opState = PAN;
  }

  return TRUE;
}

gboolean button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  rsmview->opState = NOSTATE;
  expose_event(widget, NULL, NULL);

  return TRUE;
}

gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  switch (event->keyval) {
  case GDK_Escape:
    CloseMainWindow(NULL, NULL, NULL);
    break;
  /*
  case GDK_d:
    ChangeDispMode();
    break;
  case GDK_z:
    ViewReset();
    break;
  */
  case GDK_Shift_L:
  case GDK_Shift_R:
    shiftState = true;
    break;
  default:
    return FALSE;
  }

  return TRUE;
}

void ViewReset(GtkMenuItem *menuitem, gpointer user_data)
{
  rsmview->SetCameraDefault(true);
  expose_event(oglarea, NULL, NULL);
}

void ChangeDispMode(GtkMenuItem *menuitem, gpointer user_data)
{
  rsmview->dispmode++;
  if (rsmview->dispmode == 3)  rsmview->dispmode = 0;
  expose_event(oglarea, NULL, NULL);
}

gboolean key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  switch (event->keyval) {
  case GDK_Shift_L:
  case GDK_Shift_R:
    shiftState = false;
    break;
  default:
    return FALSE;
  }

  return TRUE; 
}


void SignalConnect(void)
{
  gtk_signal_connect(GTK_OBJECT(window), "delete_event", GTK_SIGNAL_FUNC(CloseMainWindow), NULL);
  gtk_signal_connect(GTK_OBJECT(menuExit), "activate", GTK_SIGNAL_FUNC(CloseMainWindow), NULL);
  gtk_signal_connect(GTK_OBJECT(menuChangeBGCol), "activate", GTK_SIGNAL_FUNC(SelectColor), (gpointer)1);
  gtk_signal_connect(GTK_OBJECT(menuChangeLineCol), "activate", GTK_SIGNAL_FUNC(SelectColor), (gpointer)2);
  gtk_signal_connect(GTK_OBJECT(menuViewReset), "activate", GTK_SIGNAL_FUNC(ViewReset), NULL);
  gtk_widget_add_accelerator(menuViewReset, "activate", accel_group, GDK_z, (GdkModifierType)0, GTK_ACCEL_VISIBLE);
  gtk_signal_connect(GTK_OBJECT(menuDispMode), "activate", GTK_SIGNAL_FUNC(ChangeDispMode), NULL);
  gtk_widget_add_accelerator(menuDispMode, "activate", accel_group, GDK_d, (GdkModifierType)0, GTK_ACCEL_VISIBLE);
  gtk_signal_connect(GTK_OBJECT(menuMAssign), "activate", GTK_SIGNAL_FUNC(OpenDialogMouseButtonAssign), NULL);

  gtk_signal_connect(GTK_OBJECT(treeview), "cursor-changed", GTK_SIGNAL_FUNC(ModelSelect), NULL);

  gtk_widget_add_events(oglarea, GDK_BUTTON1_MOTION_MASK | GDK_BUTTON2_MOTION_MASK | GDK_BUTTON3_MOTION_MASK
                       | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_VISIBILITY_NOTIFY_MASK);
  g_signal_connect(G_OBJECT(oglarea), "configure_event", G_CALLBACK(configure_event), NULL);
  g_signal_connect(G_OBJECT(oglarea), "expose_event", G_CALLBACK(expose_event), NULL);
  g_signal_connect(G_OBJECT(oglarea), "motion_notify_event", G_CALLBACK(motion_notify_event), NULL);
  g_signal_connect(G_OBJECT(oglarea), "button_press_event", G_CALLBACK (button_press_event), NULL);
  g_signal_connect(G_OBJECT(oglarea), "button_release_event", G_CALLBACK (button_release_event), NULL);
  g_signal_connect_swapped(G_OBJECT(window), "key_press_event", G_CALLBACK(key_press_event), oglarea);
  g_signal_connect_swapped(G_OBJECT(window), "key_release_event", G_CALLBACK(key_release_event), oglarea);

}

void BuildWindow(void)
{
  GtkWidget *vbox;
  GtkWidget *menubar;
  GtkWidget *hpane;
  GtkWidget *scrolledwindow;

  accel_group = gtk_accel_group_new();
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  menubar = gtk_menu_bar_new();
  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

  BuildMenu(menubar);

  hpane = gtk_hpaned_new();
  gtk_box_pack_start(GTK_BOX(vbox), hpane, TRUE, TRUE, 0);
  gtk_paned_set_position(GTK_PANED(hpane), 200);

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_size_request(scrolledwindow, 200, 300);
  gtk_paned_pack1(GTK_PANED(hpane), scrolledwindow, FALSE, TRUE);

  treemodel = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
  BuildTree();
  treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(treemodel));
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
  gtk_tree_view_set_reorderable(GTK_TREE_VIEW(treeview), FALSE);
  gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview), FALSE);

  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "xalign", 0.0f, NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
            -1, NULL, renderer, "text", 0, NULL);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), treeview);

  oglarea = gtk_drawing_area_new();
  gtk_paned_pack2(GTK_PANED(hpane), oglarea, FALSE, TRUE);
  gtk_widget_set_size_request(oglarea, 300, 300);
  gtk_widget_set_gl_capability(oglarea, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);}

void BuildMenu(GtkWidget *menubar)
{
  GtkWidget *menuitem1;
  GtkWidget *menu1;
  GtkWidget *menuitem2;
  GtkWidget *menu2;
  GtkWidget *menuitem22;

  menuitem1 = gtk_menu_item_new_with_mnemonic(_("_File"));
  gtk_container_add(GTK_CONTAINER(menubar), menuitem1);

  menu1 = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem1), menu1);

  menuExit = gtk_menu_item_new_with_mnemonic(_("E_xit"));
  gtk_container_add(GTK_CONTAINER(menu1), menuExit);

  menuitem2 = gtk_menu_item_new_with_mnemonic(_("_View"));
  gtk_container_add(GTK_CONTAINER(menubar), menuitem2);

  menu2 = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem2), menu2);

  menuChangeBGCol = gtk_menu_item_new_with_mnemonic(_("Background color..."));
  gtk_container_add(GTK_CONTAINER(menu2), menuChangeBGCol);

  menuChangeLineCol = gtk_menu_item_new_with_mnemonic(_("Line color..."));
  gtk_container_add(GTK_CONTAINER(menu2), menuChangeLineCol);

  menuitem22 = gtk_separator_menu_item_new();
  gtk_container_add(GTK_CONTAINER(menu2), menuitem22);
  gtk_widget_set_sensitive(menuitem22, FALSE);

  menuViewReset = gtk_menu_item_new_with_mnemonic(_("view reset"));
  gtk_container_add(GTK_CONTAINER(menu2), menuViewReset);

  menuDispMode = gtk_menu_item_new_with_mnemonic(_("Change disp mode"));
  gtk_container_add(GTK_CONTAINER(menu2), menuDispMode);

  menuMAssign = gtk_menu_item_new_with_mnemonic(_("Change MouseBtn..."));
  gtk_container_add(GTK_CONTAINER(menubar), menuMAssign);
}

void BuildTree(void)
{
  for (int i=0, j=1; i<pak1->NumEntry; i++) {
    if (i == pak1->NumEntry / 10 * j) {
      g_print("tree item %d%% set...\n", j*10);
      j++;
    }
    if (pak1->Entries[i].type == 2)  continue;
    if (STRNCASECMP( &pak1->Entries[i].fn[strlen(pak1->Entries[i].fn)-4], ".rsm", 4 ) == 0) {
      /*
      g_print("%s: ", pak1->Entries[i].fn);
      rsmview->OpenRSM(pak1->Entries[i].fn);
      g_print("\n");
      */
      CreateTreeItem(NULL, pak1->Entries[i].fn);
    }
  }
}

void CreateTreeItem(GtkTreeIter *parent, char *filename)
{
  char *st;
  char buf[256];
                                                                                
  if ((st = strchr(filename, '\\')) || (st = strchr(filename, '/')) ) { // include subdir
    // dir\filename
    strncpy(buf, filename, st-filename);
    buf[st-filename] = '\0';
    CreateTreeSubtree(parent, buf, st+1);
  } else { //add item
    // st == 0
    AddTreeItem(parent, filename);
  }
}

void CreateTreeSubtree(GtkTreeIter *parent, char *subname, char *filename)
{
  GtkTreeIter iter;
  gchar *dispname, *realname;
  bool flag;
  char buf2[FILENAME_MAX];
                                                                                
  flag = false;
  if (gtk_tree_model_iter_children(GTK_TREE_MODEL(treemodel), &iter, parent)) {
    do {
      gtk_tree_model_get(GTK_TREE_MODEL(treemodel), &iter, 0, &dispname, 1, &realname, -1);
      if (strcmp(realname, subname) == 0) {
        flag = true;
        break;
      }
    } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(treemodel), &iter));
  }
                                                                                
  if (!flag) {
    gtk_tree_store_append(treemodel, &iter, parent);
#ifdef DISTRIBUTE
    strcpy(buf2, _(subname));
#else
    char buf[FILENAME_MAX];
    strcpy(buf, _(subname));
    PO_STR_SET(buf2, buf);
#endif
    gtk_tree_store_set(treemodel, &iter, 0, buf2, 1, subname, -1);
  }
  CreateTreeItem(&iter, filename);
}

void AddTreeItem(GtkTreeIter *parent, char *filename)
{
  GtkTreeIter iter;
  char subname[FILENAME_MAX];
  char buf2[FILENAME_MAX];
                                                                                
  strncpy(subname, filename, strlen(filename)-4);
  subname[strlen(filename)-4] = '\0';
  gtk_tree_store_append(treemodel, &iter, parent);
#ifdef DISTRIBUTE
  strcpy(buf2, _(subname));
#else
  char buf[FILENAME_MAX];
  strcpy(buf, _(subname));
  PO_STR_SET(buf2, buf);
#endif
  strcat(buf2, &filename[strlen(filename)-4]);
  gtk_tree_store_set(treemodel, &iter, 0, buf2, 1, filename, -1);
}

void SelectColor(GtkMenuItem *menuitem, gpointer user_data)
{
  GtkWidget *csdialog;
  csdialog = gtk_color_selection_dialog_new(NULL);
  BYTE r, g, b;
  GdkColor color;
  if ((int)user_data == 1) {  // BG
    if (rsmview == NULL)  return;
    r = (BYTE)(rsmview->bgcol.r * 0xFF);
    g = (BYTE)(rsmview->bgcol.g * 0xFF);
    b = (BYTE)(rsmview->bgcol.b * 0xFF);
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(csdialog)->colorsel), &color);
  } else if ((int)user_data == 2) {  // Line
    if (rsmview == NULL)  return;
    if (rsmview->rsm == NULL)  return;
    r = (BYTE)(rsmview->rsm->linergb.r * 0xFF);
    g = (BYTE)(rsmview->rsm->linergb.g * 0xFF);
    b = (BYTE)(rsmview->rsm->linergb.b * 0xFF);
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
  if ((int)user_data == 1) {  // BG
    rsmview->bgcol.r = r / (float)255;
    rsmview->bgcol.g = g / (float)255;
    rsmview->bgcol.b = b / (float)255;
    rsmview->InitOpenGL();
  } else if ((int)user_data == 2) {  // Line
    rsmview->rsm->linergb.r = r / (float)255;
    rsmview->rsm->linergb.g = g / (float)255;
    rsmview->rsm->linergb.b = b / (float)255;
  }
  expose_event(oglarea, NULL, NULL);
  gtk_widget_destroy(csdialog);
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

