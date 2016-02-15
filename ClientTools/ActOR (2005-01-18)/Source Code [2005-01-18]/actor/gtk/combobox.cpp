#include "combobox.h"

LIBGLADE_SIGNAL_FUNC void OnComboActChange(GtkOptionMenu *optAct, gpointer user_data)
{
  if (IsActReady() == false)  return;
  
  bool tmpPlay = autoPlay;
  if (tmpPlay) {
    OnBtnStop(NULL, NULL);
  }
  UnSelectSelection();

  int i = gtk_option_menu_get_history(optAct);

  switch (actor->GetNumAction()) {
  case 1:
    SetStateDirecButton(0x00);
    break;
  case 2:
    SetStateDirecButton(0x82);
    break;
  case 4:
    SetStateDirecButton(0xAA);
    break;
  default:
    if ((i+1)*8 <= actor->GetNumAction()) {
      SetStateDirecButton(0xFF);
    } else {
      BYTE f = 0;
      for (int i=0; i<actor->GetNumAction() % 8; i++) {
        f <<= 1;
        f |= 1;
      }
      SetStateDirecButton(f);
    }
    break;
  }

  actor->curAct = i*8;
  actor->curDir = 0;
  actor->curPat = 0;

  if (tmpPlay) {
    OnBtnPlay(NULL, NULL);
  }

  ProcPatternChange(true);
}

LIBGLADE_SIGNAL_FUNC void OnComboSoundChange(GtkOptionMenu *optSound, gpointer user_data)
{
  if (actor->act == NULL) return;
  if (autoPlay)  return;

  int no = gtk_option_menu_get_history(optSound);

  actor->SetSoundNo(actor->curAct, actor->curPat, no-1);
}
