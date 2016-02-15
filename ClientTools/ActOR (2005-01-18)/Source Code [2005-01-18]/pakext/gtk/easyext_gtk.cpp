#include "easyext_gtk.h"

extern CPAK *pak1;
extern MY_FLAGS flag;

GtkWidget *windowEE;
GtkTreeStore *model;
GtkWidget *treeview;
GtkWidget *btnCloseEE;
GtkWidget *btnToChrV;
GtkWidget *btnAnyProc;
GtkWidget *btnExtract;
GtkWidget *btnMakePatch1;
GtkWidget *btnMakePatch2;
char EEpath[FILENAME_MAX];
GtkWidget *hbox1;
GtkWidget *viewEE;
GtkWidget *scrEE = NULL;

int returnEE;

#define PAKTEMP "pakext_temp"
static char suffixEnableProc[][4] = {"bmp", "tga", "jpg", "txt", "xml", "spr"};


int EasyExt(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  EEBuildWindow();
  EESignalConnect();
  gtk_widget_show_all(windowEE);

  gtk_main();

  EEExitDialog();

  return returnEE;
}

void EEExitDialog(void)
{
  gtk_widget_destroy(windowEE);

  char fname[FILENAME_MAX];
  for (int i=0; i<sizeof(suffixEnableProc) / 4; i++) {
    SNPRINTF(fname, sizeof(fname), "%s.%s", PAKTEMP, suffixEnableProc[i]);
    remove(fname);
  }
}

void EEClickBtnExtract(GtkButton *button, gpointer user_data)
{
  GtkTreeIter iter;
  GtkTreeModel *mod;

  if (EEFindTreeIter(&iter, &mod) == false)  return;
  if (EEGetFilename(_("Select output directory"), false, true) == false)  return;
  CHDIR(EEpath);
  EEExtractMain(iter, mod);
}

bool EEFindTreeIter(GtkTreeIter *iter, GtkTreeModel **mod)
{
  GtkTreeSelection *selection;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  if (!gtk_tree_selection_get_selected(selection, mod, iter))  return false;

  return true;
}

bool EEGetFilename(char *title, bool open, bool dir)
{
  GtkWidget *fc;
  if (dir) {
    fc = gtk_file_chooser_dialog_new(title, GTK_WINDOW(windowEE),
           GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
           GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  } else {
    if (open) {
      fc = gtk_file_chooser_dialog_new(title, GTK_WINDOW(windowEE),
             GTK_FILE_CHOOSER_ACTION_OPEN,
             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
             GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    } else {
      fc = gtk_file_chooser_dialog_new(title, GTK_WINDOW(windowEE),
             GTK_FILE_CHOOSER_ACTION_SAVE,
             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
             GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
    }
  }
                                                                                
  strcpy(EEpath, "");
  bool ret;
  if (gtk_dialog_run(GTK_DIALOG(fc)) == GTK_RESPONSE_ACCEPT) {
    strcpy(EEpath, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc)));
    ret = true;
  } else {
    ret = false;
  }
  gtk_widget_destroy(fc);
                                                                                
  return ret;
}

void EEExtractMain(GtkTreeIter iter, GtkTreeModel *mod)
{
  if (gtk_tree_model_iter_has_child(mod, &iter)) {
    EEExtractChild(iter, mod);
  } else {
    EEExtractFile(iter, mod);
  }
}

void EEExtractChild(GtkTreeIter iter, GtkTreeModel *mod)
{
  GtkTreeIter child;
  char curdir[FILENAME_MAX];
  char *name = NULL;

  GETCWD(curdir, sizeof(curdir));
  gtk_tree_model_get(mod, &iter, 0, &name, -1);
  MKDIR(name);
  CHDIR(name);
  int numChild;
  numChild = gtk_tree_model_iter_n_children(mod, &iter);
  for (int i=0; i<numChild; i++) {
    gtk_tree_model_iter_nth_child(mod, &child, &iter, i);
    EEExtractMain(child, mod);
  }
  CHDIR(curdir);
}

void EEExtractFile(GtkTreeIter iter, GtkTreeModel *mod)
{
  char fname[FILENAME_MAX];
  char *name = NULL;

  gtk_tree_model_get(mod, &iter, 0, &name, -1);

  EEMakeOrigFilename(fname, iter, mod);
  printf("Now Extract %s\n", fname);
  pak1->Uncomp2tmp(fname);

  FILE *fp;
  fp = fopen(name, "wb");
  if (fp == NULL)  return;
  fwrite(pak1->tdata, 1, pak1->tlen, fp);
  fclose(fp);
}

void EEMakeOrigFilename(char *fname, GtkTreeIter iter, GtkTreeModel *mod)
{
  GtkTreeIter parent;
  char buf[FILENAME_MAX], buf2[FILENAME_MAX];
  char *name = NULL;

  gtk_tree_model_get(mod, &iter, 1, &name, -1);
  strcpy(buf, name);
  while (gtk_tree_model_iter_parent(mod, &parent, &iter)) {
    gtk_tree_model_get(mod, &parent, 1, &name, -1);
    strcpy(buf2, name);
    sprintf(fname, "%s\\%s", buf2, buf);
    strcpy(buf, fname);
    iter = parent;
  }
}

#ifdef VIOLATION_CODE
void EEMakePatch(GtkButton *button, gpointer user_data)
{
  char srcfn[FILENAME_MAX];
  char destfn[FILENAME_MAX];
  char origfn[FILENAME_MAX];

  GtkTreeIter iter;
  GtkTreeModel *mod;

  if (EEFindTreeIter(&iter, &mod) == false)  return;
  if (gtk_tree_model_iter_has_child(mod, &iter)) {
    GtkWidget *mes;
    mes = gtk_message_dialog_new(GTK_WINDOW(windowEE), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                 _("Select target FILE from tree.\n(Don't select directory)"));
    gtk_dialog_run(GTK_DIALOG(mes));
    gtk_widget_destroy(mes);
    return;
  }

  if (EEGetFilename(_("Select file. This file will be patch file."), true, false) == false) {
    return;
  } else {
    strncpy(srcfn, EEpath, sizeof(srcfn));
  }
  if (EEGetFilename(_("Set save file. i.e. update.gpf"), false, false) == false) {
    return;
  } else {
    strncpy(destfn, EEpath, sizeof(destfn));
  }

  if ((int)user_data == 1) {
    pak1->MakePatchOpen(destfn, 0x102);
  } else if ((int)user_data == 2) {
    pak1->MakePatchOpen(destfn, 0x200);
  }

  FILE *fp;
  BYTE *dat;
  int len;

  fp = fopen(srcfn, "rb");
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  dat = (BYTE *)malloc(len);
  fread(dat, 1, len, fp);
  fclose(fp);
  EEMakeOrigFilename(origfn, iter, mod);
  pak1->MakePatchAdd(origfn, dat, len);
  pak1->MakePatchClose();
  free(dat);
}
#endif  // VIOLATION_CODE

bool EEExtractTemp(char *tmpfname, char *origfname)
{
  FILE *fp;
  int flen;
                                                                                
  if ((fp = fopen(tmpfname, "wb")) == NULL)  return false;
  pak1->Uncomp2tmp(origfname);
  flen = pak1->tlen;
  fwrite(pak1->tdata, 1, flen, fp);
  fclose(fp);
                                                                                
  return true;
}

gint EECloseDialog(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  gtk_main_quit();

  returnEE = 0;

  return TRUE;
}

void EECloseDialogBtn(GtkButton *button, gpointer data)
{
  gtk_main_quit();

  returnEE = (int)data;
}

void EEAnyProc__(GtkButton *button, gpointer user_data)
{
  char fname[FILENAME_MAX];

  GtkTreeIter iter;
  GtkTreeModel *mod;

  if (EEFindTreeIter(&iter, &mod) == false)  return;
  if (gtk_tree_model_iter_has_child(mod, &iter)) {
    GtkWidget *mes;
    mes = gtk_message_dialog_new(GTK_WINDOW(windowEE), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                 _("Select target FILE from tree.\n(Don't select directory)"));
    gtk_dialog_run(GTK_DIALOG(mes));
    gtk_widget_destroy(mes);
    return;
  }

  EEMakeOrigFilename(fname, iter, mod);
  char efname[FILENAME_MAX];
  for (int i=0; i<sizeof(suffixEnableProc) / 4; i++) {
    if (STRNCASECMP(&fname[strlen(fname)-3], suffixEnableProc[i], 3) == 0) {
      SNPRINTF(efname, sizeof(efname), "%s.%s", PAKTEMP, suffixEnableProc[i]);
      if (EEExtractTemp(efname, fname) == false)  return;
      if ( (STRCASECMP(suffixEnableProc[i], "bmp") == 0) 
        || (STRCASECMP(suffixEnableProc[i], "jpg") == 0)
        || (STRCASECMP(suffixEnableProc[i], "tga") == 0) ) {
        if (scrEE != NULL) {
          gtk_widget_destroy(scrEE);
        }
        scrEE = gtk_scrolled_window_new(NULL, NULL);
        gtk_box_pack_start(GTK_BOX(hbox1), scrEE, TRUE, TRUE, 0);

        viewEE = gtk_image_new();
        gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrEE), viewEE);
        gtk_image_set_from_file(GTK_IMAGE(viewEE), efname);
        gtk_widget_show_all(scrEE);
        return;
      }
      if ( (STRCASECMP(suffixEnableProc[i], "txt") == 0)
        || (STRCASECMP(suffixEnableProc[i], "xml") == 0) ) {
        if (scrEE != NULL) {
          gtk_widget_destroy(scrEE);
        }
        scrEE = gtk_scrolled_window_new(NULL, NULL);
        gtk_box_pack_start(GTK_BOX(hbox1), scrEE, TRUE, TRUE, 0);

        viewEE = gtk_text_view_new();
        gtk_container_add(GTK_CONTAINER(scrEE), viewEE);
        GtkTextBuffer *buffer;
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(viewEE));

        gchar *nbuf = new gchar[1024];
        gchar *utf8buf = new gchar[2048];
        FILE *fp;
        fp = fopen(efname, "r");
        while (feof(fp) == 0) {
          fgets(nbuf, 1024, fp);
          CP932toUTF8(utf8buf, 2048, nbuf, strlen(nbuf));
          gtk_text_buffer_insert_at_cursor(buffer, utf8buf, -1);
        }
        fclose(fp);

        gtk_widget_show_all(scrEE);
        delete nbuf, utf8buf;
        return;
      }
      if ( (STRCASECMP(suffixEnableProc[i], "spr") == 0) ) {
        EXECLP("actor", "actor", efname, NULL);
        return;
      }
    }
  }
}

void EESignalConnect(void)
{
  gtk_signal_connect(GTK_OBJECT(windowEE), "delete_event", GTK_SIGNAL_FUNC(EECloseDialog), NULL);
  gtk_signal_connect(GTK_OBJECT(btnCloseEE), "clicked", GTK_SIGNAL_FUNC(EECloseDialogBtn), (gpointer)0);
  gtk_signal_connect(GTK_OBJECT(btnToChrV), "clicked", GTK_SIGNAL_FUNC(EECloseDialogBtn), (gpointer)2);
  gtk_signal_connect(GTK_OBJECT(btnAnyProc), "clicked", GTK_SIGNAL_FUNC(EEAnyProc__), NULL);
  gtk_signal_connect(GTK_OBJECT(btnExtract), "clicked", GTK_SIGNAL_FUNC(EEClickBtnExtract), NULL);
#ifdef VIOLATION_CODE
  gtk_signal_connect(GTK_OBJECT(btnMakePatch1), "clicked", GTK_SIGNAL_FUNC(EEMakePatch), (gpointer)1);
  gtk_signal_connect(GTK_OBJECT(btnMakePatch2), "clicked", GTK_SIGNAL_FUNC(EEMakePatch), (gpointer)2);
#endif  // VIOLATION_CODE
}

void EEBuildWindow(void)
{
  GtkWidget *hbox2;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *alignment1;
  GtkWidget *alignment2;
  GtkWidget *alignment3;
#ifdef VIOLATION_CODE
  GtkWidget *alignment4;
  GtkWidget *alignment5;
#endif
  GtkWidget *alignment6;
  GtkWidget *alignment7;
  GtkWidget *image1;
  GtkWidget *scrolledwindow1;

  windowEE = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  hbox1 = gtk_hbox_new(FALSE, 8);
  gtk_container_add(GTK_CONTAINER(windowEE), hbox1);

  scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_size_request(scrolledwindow1, 100, 200);
  gtk_box_pack_start(GTK_BOX(hbox1), scrolledwindow1, TRUE, TRUE, 0);

  model = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
  EEBuildTree();
  treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
  gtk_tree_view_set_reorderable(GTK_TREE_VIEW(treeview), FALSE);
  gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview), TRUE);

  GtkCellRenderer *renderer;
  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "xalign", 0.0f, NULL);
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
            -1, NULL, renderer, "text", 0, NULL);
  gtk_container_add(GTK_CONTAINER(scrolledwindow1), treeview);


  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX (hbox1), vbox1, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new(1, 0, 0.3f, 0.2f);
  gtk_box_pack_start(GTK_BOX(vbox1), alignment1, TRUE, TRUE, 0);

  btnCloseEE = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(alignment1), btnCloseEE);

  alignment2 = gtk_alignment_new(0.5f, 0.5f, 0, 0);
  gtk_container_add(GTK_CONTAINER(btnCloseEE), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_container_add(GTK_CONTAINER(alignment2), hbox2);

  image1 = gtk_image_new_from_stock("gtk-quit", GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start(GTK_BOX(hbox2), image1, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic(_("Close"));
  gtk_box_pack_start(GTK_BOX(hbox2), label1, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label1), GTK_JUSTIFY_LEFT);

  alignment6 = gtk_alignment_new(1, 0, 0.3f, 0.2f);
  gtk_box_pack_start(GTK_BOX(vbox1), alignment6, TRUE, TRUE, 0);

  btnToChrV = gtk_button_new_with_mnemonic(_("To ChrViewer"));
  gtk_container_add(GTK_CONTAINER(alignment6), btnToChrV);

  alignment7 = gtk_alignment_new(1, 0, 0.3f, 0.2f);
  gtk_box_pack_start(GTK_BOX(vbox1), alignment7, TRUE, TRUE, 0);

  btnAnyProc = gtk_button_new_with_mnemonic(_("AnyProc"));
  gtk_container_add(GTK_CONTAINER(alignment7), btnAnyProc);

  alignment3 = gtk_alignment_new(0.5f, 0.5f, 0.5f, 0.5f);
  gtk_box_pack_start(GTK_BOX(vbox1), alignment3, TRUE, TRUE, 0);

  btnExtract = gtk_button_new_with_mnemonic(_("Extract"));
  gtk_container_add(GTK_CONTAINER(alignment3), btnExtract);

#ifdef VIOLATION_CODE
  alignment4 = gtk_alignment_new(0.5f, 0.8f, 0.5f, 0.5f);
  gtk_box_pack_start(GTK_BOX(vbox1), alignment4, TRUE, TRUE, 0);

  btnMakePatch1 = gtk_button_new_with_mnemonic(_("Make Patch\nVer 0x01xx"));
  gtk_container_add(GTK_CONTAINER(alignment4), btnMakePatch1);

  alignment5 = gtk_alignment_new(0.5f, 0.2f, 0.5f, 0.5f);
  gtk_box_pack_start(GTK_BOX(vbox1), alignment5, TRUE, TRUE, 0);

  btnMakePatch2 = gtk_button_new_with_mnemonic(_("Make Patch\nVer 0x02xx"));
  gtk_container_add(GTK_CONTAINER(alignment5), btnMakePatch2);
#endif  // VIOLATION_CODE
}

void EEBuildTree(void)
{
  g_print("Is this slow? If you think so, please use -m option.\n");
  for (int i=0, j=1; i<pak1->NumEntry; i++) {
    if (i == pak1->NumEntry / 10 * j) {
      g_print("tree item %d%% set...\n", j*10);
      j++;
    }
    if (pak1->Entries[i].type == 2)  continue;
    if (flag.useMatch) {
      if (strstr( pak1->Entries[i].fn, flag.strMatch ) == NULL)  continue;
    }
    EECreateItem(NULL, pak1->Entries[i].fn);
  }
}

void EECreateItem(GtkTreeIter *parent, char *filename)
{
  char *st;
  char buf[256];

  if ((st = strchr(filename, '\\')) || (st = strchr(filename, '/')) ) { // include subdir
    // dir\filename
    strncpy(buf, filename, st-filename);
    buf[st-filename] = '\0';
    EECreateSubtree(parent, buf, st+1);
  } else { //add item
    // st == 0 
    EEAddItem(parent, filename);
  }
}

void EECreateSubtree(GtkTreeIter *parent, char *subname, char *filename)
{
  GtkTreeIter iter;
  gchar *dispname, *realname;
  bool flag;
  char buf2[FILENAME_MAX];

  flag = false;
  if (gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &iter, parent)) {
    do {
      gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &dispname, 1, &realname, -1);
      if (strcmp(realname, subname) == 0) {
        flag = true;
        break;
      }
    } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &iter));
  }

  if (!flag) {
    gtk_tree_store_append(model, &iter, parent);
#ifdef DISTRIBUTE
    strcpy(buf2, _(subname));
#else
    char buf[FILENAME_MAX];
    strcpy(buf, _(subname));
    PO_STR_SET(buf2, buf);
#endif
    gtk_tree_store_set(model, &iter, 0, buf2, 1, subname, -1);
  }
  EECreateItem(&iter, filename);
}

void EEAddItem(GtkTreeIter *parent, char *filename)
{
  GtkTreeIter iter;
  char subname[FILENAME_MAX];
  char buf2[FILENAME_MAX];

  strncpy(subname, filename, strlen(filename)-4);
  subname[strlen(filename)-4] = '\0';
  gtk_tree_store_append(model, &iter, parent);
#ifdef DISTRIBUTE
  strcpy(buf2, _(subname));
#else
  char buf[FILENAME_MAX];
  strcpy(buf, _(subname));
  PO_STR_SET(buf2, buf);
#endif
  strcat(buf2, &filename[strlen(filename)-4]);
  gtk_tree_store_set(model, &iter, 0, buf2, 1, filename, -1);
}

