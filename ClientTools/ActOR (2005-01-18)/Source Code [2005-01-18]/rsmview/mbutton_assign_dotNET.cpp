#include "stdafx.h"
#include "mbutton_assign_dotNET.h"

#include <windows.h>

void rsmview::mbutton_assign_dotNET::SetMouseAssign(void)
{
  if (DialogResult == DialogResult::Cancel)  return;
  if (this->rb11->Checked)  mouseAssign->pan = MK_LBUTTON;
  if (this->rb12->Checked)  mouseAssign->rot = MK_LBUTTON;
  if (this->rb13->Checked)  mouseAssign->zoom = MK_LBUTTON;
  if (this->rb21->Checked)  mouseAssign->pan = MK_MBUTTON;
  if (this->rb22->Checked)  mouseAssign->rot = MK_MBUTTON;
  if (this->rb23->Checked)  mouseAssign->zoom = MK_MBUTTON;
  if (this->rb31->Checked)  mouseAssign->pan = MK_RBUTTON;
  if (this->rb32->Checked)  mouseAssign->rot = MK_RBUTTON;
  if (this->rb33->Checked)  mouseAssign->zoom = MK_RBUTTON;
}

void rsmview::mbutton_assign_dotNET::SetRadioButton(void)
{
  if (mouseAssign->pan == MK_LBUTTON)  this->rb11->Checked = true;
  if (mouseAssign->rot == MK_LBUTTON)  this->rb12->Checked = true;
  if (mouseAssign->zoom == MK_LBUTTON)  this->rb13->Checked = true;
  if (mouseAssign->pan == MK_MBUTTON)  this->rb21->Checked = true;
  if (mouseAssign->rot == MK_MBUTTON)  this->rb22->Checked = true;
  if (mouseAssign->zoom == MK_MBUTTON)  this->rb23->Checked = true;
  if (mouseAssign->pan == MK_RBUTTON)  this->rb31->Checked = true;
  if (mouseAssign->rot == MK_RBUTTON)  this->rb32->Checked = true;
  if (mouseAssign->zoom == MK_RBUTTON)  this->rb33->Checked = true;
}

void rsmview::mbutton_assign_dotNET::CheckRadioButtons(int id)
{
  if (MACheckThrough)  return;

  RadioButton *ar[];
  ar = new RadioButton *[9];
  ar[0] = rb11; ar[1] = rb12; ar[2] = rb13;
  ar[3] = rb21; ar[4] = rb22; ar[5] = rb23;
  ar[6] = rb31; ar[7] = rb32; ar[8] = rb33;

  int id1 = id / 3;
  int ca[3][2] = {{1, 2}, {-1, 1}, {-2, -1}};
  int cb[3][2] = {{3, 6}, {-3, 3}, {-6, -3}};

  int i;
  // exclusive procedure
  if (ar[ id+cb[id1][0] ]->Checked) {
    for (i=0; i<2; i++) {
      if (ar[ id+cb[id1][1]+ca[id%3][i] ]->Checked == false) {
        break;
      }
    }
    MACheckThrough = true;
    ar[ id+cb[id1][0]+ca[id%3][i] ]->Checked = true;
    MACheckThrough = false;
  } else if (ar[ id+cb[id1][1] ]->Checked) {
    for (i=0; i<2; i++) {
      if (ar[ id+cb[id1][0]+ca[id%3][i] ]->Checked == false) {
        break;
      }
    }
    MACheckThrough = true;
    ar[ id+cb[id1][1]+ca[id%3][i] ]->Checked = true;
    MACheckThrough = false;
  }
}
