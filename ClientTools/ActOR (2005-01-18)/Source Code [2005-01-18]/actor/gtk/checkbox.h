#ifndef __MY_CHECKBOX__
#define __MY_CHECKBOX__

#include <gtk/gtk.h>

#include "../main.h"

LIBGLADE_SIGNAL_FUNC void OnBtnHead(GtkToggleButton *togglebutton, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnBtnBody(GtkToggleButton *togglebutton, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnBtnEtc(GtkToggleButton *togglebutton, gpointer user_data);
LIBGLADE_SIGNAL_FUNC void OnBtnNeigh(GtkToggleButton *togglebutton, gpointer user_data);

void CheckRefHead(void);
void CheckRefBody(void);
void CheckRefEtc(void);
void CheckRefNeigh(void);

#endif // __MY_CHECKBOX__

