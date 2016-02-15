#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <direct.h>

#include "rsmview.h"
#include "../common/global.h"
#include "../common/pak.h"

#include "mbutton_assign_dotNET.h"
extern MOUSE_ASSIGN *mouseAssign;

typedef struct {
  int root;
  int handle;
  char *disp;
  char *file;
  int child;
  int next;
} LOCALTREE;

#define EVERY_LOCALTREE 1000

using namespace System::Runtime::InteropServices;

namespace rsmview
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Form1 の概要
	///
	/// 警告 : このクラスの名前を変更する場合、このクラスが依存するすべての .resx ファイルに関連付けられた 
	///          マネージ リソース コンパイラ ツールに対して 'Resource File Name' プロパティを
	///          変更する必要があります。この変更を行わないと、
	///          デザイナと、このフォームに関連付けられたローカライズ済みリソースとが正しく相互に利用できなくなります。
	/// </summary>
	public __gc class FrmMain : public System::Windows::Forms::Form
	{	
  private:
    bool pakread;
    bool firstpaint;
    HGLRC hglrc;
    HDC hdc;
    CPAK *pak1;
    CRsmview *rsmview;
    bool shiftState;

    LOCALTREE *ltree;
    int numltree;

  private: System::Windows::Forms::MenuItem *  menuExit;
  private: System::Windows::Forms::MenuItem *  menuOpen;
  private: System::Windows::Forms::MenuItem *  menuItem2;
  private: System::Windows::Forms::OpenFileDialog *  openFileDialog1;
  private: System::Windows::Forms::MenuItem *  menuItem3;
  private: System::Windows::Forms::MenuItem *  menuDispChange;
  private: System::Windows::Forms::MenuItem *  menuAddPak;
  private: System::Windows::Forms::MenuItem *  menuViewReset;
  private: System::Windows::Forms::MenuItem *  menuItem4;
  private: System::Windows::Forms::MenuItem *  menuSaveAsX;
  private: System::Windows::Forms::SaveFileDialog *  saveFileDialog1;
  private: System::Windows::Forms::MenuItem *  menuItem5;
  private: System::Windows::Forms::MenuItem *  menuChangeBGCol;
  private: System::Windows::Forms::MenuItem *  menuChangeLineCol;
  private: System::Windows::Forms::ColorDialog *  colorDialog1;


  private: System::Windows::Forms::MenuItem *  menuMouseAssign;

    void MainClose(void);
    void InitLocalValues(void);
    void SetTextDomain(char *progname);
    void SetupOpenGLWin(void);
    bool OpenPak(char *fname);
    void BuildTree(void);
    void SetRootLocalTree(void);
    int SetRootTree(void);
    void SetItemDirCheck(int handle, int lno, char *fname);
    void SetItemMakeBranch(int handle, int lno, char *node, char *fname);
    int AddTreeItem(int parent, char *name);
    int AddLocalTreeItem(int root, int handle, char *disp, char *file);
    bool SetItemMakeBranchIsExist(int handle, int *c_handle, int lno, int *lno_c, char *ch_n);
    int SetItemFinalItem(int handle, char *fname);
    void SetItemLocalFinalItem(int handle, int lno, char *fname);

    void TreeViewSelect(int handle);
    bool TreeViewSelectCheck(int handle, LOCALTREE **rt);
    void MakeOrigFilename(char *fname, LOCALTREE *lt);

    void Draw(void);
    void OGLResize(void);

	public:
		FrmMain(void)
		{
			InitializeComponent();
      InitLocalValues();
		}
  
	protected:
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
  private: System::Windows::Forms::MainMenu *  mainMenu1;
  private: System::Windows::Forms::TreeView *  treeView1;
  private: System::Windows::Forms::Splitter *  splitter1;
  private: System::Windows::Forms::PictureBox *  pictureBox1;
  private: System::Windows::Forms::MenuItem *  menuItem1;



	private:
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		System::ComponentModel::Container * components;

		/// <summary>
		/// デザイナ サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディタで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
      this->mainMenu1 = new System::Windows::Forms::MainMenu();
      this->menuItem1 = new System::Windows::Forms::MenuItem();
      this->menuOpen = new System::Windows::Forms::MenuItem();
      this->menuAddPak = new System::Windows::Forms::MenuItem();
      this->menuItem2 = new System::Windows::Forms::MenuItem();
      this->menuSaveAsX = new System::Windows::Forms::MenuItem();
      this->menuItem4 = new System::Windows::Forms::MenuItem();
      this->menuExit = new System::Windows::Forms::MenuItem();
      this->menuItem3 = new System::Windows::Forms::MenuItem();
      this->menuViewReset = new System::Windows::Forms::MenuItem();
      this->menuDispChange = new System::Windows::Forms::MenuItem();
      this->menuItem5 = new System::Windows::Forms::MenuItem();
      this->menuChangeBGCol = new System::Windows::Forms::MenuItem();
      this->menuChangeLineCol = new System::Windows::Forms::MenuItem();
      this->menuMouseAssign = new System::Windows::Forms::MenuItem();
      this->treeView1 = new System::Windows::Forms::TreeView();
      this->splitter1 = new System::Windows::Forms::Splitter();
      this->pictureBox1 = new System::Windows::Forms::PictureBox();
      this->openFileDialog1 = new System::Windows::Forms::OpenFileDialog();
      this->saveFileDialog1 = new System::Windows::Forms::SaveFileDialog();
      this->colorDialog1 = new System::Windows::Forms::ColorDialog();
      this->SuspendLayout();
      // 
      // mainMenu1
      // 
      System::Windows::Forms::MenuItem* __mcTemp__1[] = new System::Windows::Forms::MenuItem*[3];
      __mcTemp__1[0] = this->menuItem1;
      __mcTemp__1[1] = this->menuItem3;
      __mcTemp__1[2] = this->menuMouseAssign;
      this->mainMenu1->MenuItems->AddRange(__mcTemp__1);
      // 
      // menuItem1
      // 
      this->menuItem1->Index = 0;
      System::Windows::Forms::MenuItem* __mcTemp__2[] = new System::Windows::Forms::MenuItem*[6];
      __mcTemp__2[0] = this->menuOpen;
      __mcTemp__2[1] = this->menuAddPak;
      __mcTemp__2[2] = this->menuItem2;
      __mcTemp__2[3] = this->menuSaveAsX;
      __mcTemp__2[4] = this->menuItem4;
      __mcTemp__2[5] = this->menuExit;
      this->menuItem1->MenuItems->AddRange(__mcTemp__2);
      this->menuItem1->Text = S"ファイル(&F)";
      // 
      // menuOpen
      // 
      this->menuOpen->Index = 0;
      this->menuOpen->Text = S"開く...(&O)";
      this->menuOpen->Click += new System::EventHandler(this, menuOpen_Click);
      // 
      // menuAddPak
      // 
      this->menuAddPak->Enabled = false;
      this->menuAddPak->Index = 1;
      this->menuAddPak->Text = S"追加...";
      this->menuAddPak->Click += new System::EventHandler(this, menuAddPak_Click);
      // 
      // menuItem2
      // 
      this->menuItem2->Index = 2;
      this->menuItem2->Text = S"-";
      // 
      // menuSaveAsX
      // 
      this->menuSaveAsX->Enabled = false;
      this->menuSaveAsX->Index = 3;
      this->menuSaveAsX->Text = S"X形式で保存...";
      this->menuSaveAsX->Click += new System::EventHandler(this, menuSaveAsX_Click);
      // 
      // menuItem4
      // 
      this->menuItem4->Index = 4;
      this->menuItem4->Text = S"-";
      // 
      // menuExit
      // 
      this->menuExit->Index = 5;
      this->menuExit->Text = S"終了(&X)";
      this->menuExit->Click += new System::EventHandler(this, menuExit_Click);
      // 
      // menuItem3
      // 
      this->menuItem3->Index = 1;
      System::Windows::Forms::MenuItem* __mcTemp__3[] = new System::Windows::Forms::MenuItem*[5];
      __mcTemp__3[0] = this->menuViewReset;
      __mcTemp__3[1] = this->menuDispChange;
      __mcTemp__3[2] = this->menuItem5;
      __mcTemp__3[3] = this->menuChangeBGCol;
      __mcTemp__3[4] = this->menuChangeLineCol;
      this->menuItem3->MenuItems->AddRange(__mcTemp__3);
      this->menuItem3->Text = S"表示(&V)";
      // 
      // menuViewReset
      // 
      this->menuViewReset->Index = 0;
      this->menuViewReset->Shortcut = System::Windows::Forms::Shortcut::CtrlZ;
      this->menuViewReset->Text = S"リセット";
      this->menuViewReset->Click += new System::EventHandler(this, menuViewReset_Click);
      // 
      // menuDispChange
      // 
      this->menuDispChange->Index = 1;
      this->menuDispChange->Shortcut = System::Windows::Forms::Shortcut::CtrlD;
      this->menuDispChange->Text = S"線・面";
      this->menuDispChange->Click += new System::EventHandler(this, menuDispChange_Click);
      // 
      // menuItem5
      // 
      this->menuItem5->Index = 2;
      this->menuItem5->Text = S"-";
      // 
      // menuChangeBGCol
      // 
      this->menuChangeBGCol->Index = 3;
      this->menuChangeBGCol->Text = S"背景色変更...";
      this->menuChangeBGCol->Click += new System::EventHandler(this, menuChangeBGCol_Click);
      // 
      // menuChangeLineCol
      // 
      this->menuChangeLineCol->Index = 4;
      this->menuChangeLineCol->Text = S"線色変更...";
      this->menuChangeLineCol->Click += new System::EventHandler(this, menuChangeLineCol_Click);
      // 
      // menuMouseAssign
      // 
      this->menuMouseAssign->Index = 2;
      this->menuMouseAssign->Text = S"マウスボタン変更(&M)";
      this->menuMouseAssign->Click += new System::EventHandler(this, menuMouseAssign_Click);
      // 
      // treeView1
      // 
      this->treeView1->Dock = System::Windows::Forms::DockStyle::Left;
      this->treeView1->ImageIndex = -1;
      this->treeView1->Location = System::Drawing::Point(0, 0);
      this->treeView1->Name = S"treeView1";
      this->treeView1->SelectedImageIndex = -1;
      this->treeView1->Size = System::Drawing::Size(121, 159);
      this->treeView1->TabIndex = 0;
      this->treeView1->KeyDown += new System::Windows::Forms::KeyEventHandler(this, treeView1_KeyDown);
      this->treeView1->KeyUp += new System::Windows::Forms::KeyEventHandler(this, treeView1_KeyUp);
      this->treeView1->AfterSelect += new System::Windows::Forms::TreeViewEventHandler(this, treeView1_AfterSelect);
      // 
      // splitter1
      // 
      this->splitter1->BackColor = System::Drawing::SystemColors::ControlDark;
      this->splitter1->Location = System::Drawing::Point(121, 0);
      this->splitter1->Name = S"splitter1";
      this->splitter1->Size = System::Drawing::Size(5, 159);
      this->splitter1->TabIndex = 1;
      this->splitter1->TabStop = false;
      // 
      // pictureBox1
      // 
      this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
      this->pictureBox1->Location = System::Drawing::Point(126, 0);
      this->pictureBox1->Name = S"pictureBox1";
      this->pictureBox1->Size = System::Drawing::Size(166, 159);
      this->pictureBox1->TabIndex = 2;
      this->pictureBox1->TabStop = false;
      this->pictureBox1->Resize += new System::EventHandler(this, pictureBox1_Resize);
      this->pictureBox1->Paint += new System::Windows::Forms::PaintEventHandler(this, pictureBox1_Paint);
      this->pictureBox1->MouseUp += new System::Windows::Forms::MouseEventHandler(this, pictureBox1_MouseUp);
      this->pictureBox1->MouseMove += new System::Windows::Forms::MouseEventHandler(this, pictureBox1_MouseMove);
      this->pictureBox1->MouseDown += new System::Windows::Forms::MouseEventHandler(this, pictureBox1_MouseDown);
      // 
      // FrmMain
      // 
      this->AutoScaleBaseSize = System::Drawing::Size(5, 12);
      this->ClientSize = System::Drawing::Size(292, 159);
      this->Controls->Add(this->pictureBox1);
      this->Controls->Add(this->splitter1);
      this->Controls->Add(this->treeView1);
      this->Menu = this->mainMenu1;
      this->Name = S"FrmMain";
      this->Text = S"RSMviewer";
      this->Closed += new System::EventHandler(this, FrmMain_Closed);
      this->Paint += new System::Windows::Forms::PaintEventHandler(this, FrmMain_Paint);
      this->ResumeLayout(false);

    }	
  private: System::Void pictureBox1_Paint(Object* sender, System::Windows::Forms::PaintEventArgs* e) 
           {
             Draw();
           }
  private: System::Void pictureBox1_Resize(Object* sender, System::EventArgs* e) 
           {
             OGLResize();
           }
  private: System::Void FrmMain_Paint(Object* sender, System::Windows::Forms::PaintEventArgs* e) 
           {
             if (firstpaint) {
               if (pakread) {
                 BuildTree();
               }
               SetupOpenGLWin();
               firstpaint = false;
             }
             this->treeView1->Focus();
           }
  private: System::Void FrmMain_Closed(Object* sender, System::EventArgs* e) 
           {
             MainClose();
           }
  private: System::Void menuOpen_Click(Object* sender, System::EventArgs* e) 
           {
             char cdir[MAX_PATH];
             GETCWD(cdir, sizeof(cdir));
             this->openFileDialog1->set_Filter(S"grf file (*.grf)|*.grf|gpf file (*.gpf)|*.gpf|pak file (*.pak)|*.pak|all files (*.*)|*.*" );
             if (this->openFileDialog1->ShowDialog() == DialogResult::OK) {
               CHDIR(cdir);
               if (pak1)  delete pak1;
               if (rsmview)  delete rsmview;
               OpenPak((char *)Marshal::StringToHGlobalAnsi(this->openFileDialog1->FileName).ToPointer());
               treeView1->Nodes->Clear();
               BuildTree();
               OGLResize();
               this->menuAddPak->Enabled = true;
             } else {
               CHDIR(cdir);
             }
           }
  private: System::Void menuExit_Click(Object* sender, System::EventArgs* e) 
           {
             this->Close();
           }
  private: System::Void menuMouseAssign_Click(Object* sender, System::EventArgs* e)
           {
             mbutton_assign_dotNET *FrmMB = new mbutton_assign_dotNET();
             FrmMB->ShowDialog();
           }
  private: System::Void pictureBox1_MouseMove(System::Object* sender, System::Windows::Forms::MouseEventArgs*  e)
           {
             if (rsmview == NULL)  return;

             rsmview->new_mp.x = e->X;
             rsmview->new_mp.y = e->Y;
             switch (rsmview->opState) {
             case NOSTATE:
               break;
             case OBJ_ROT:
               rsmview->GLOBJRot();
               Draw();
               break;
             case SCRN_ROT:
               rsmview->GLSCRNRot();
               Draw();
               break;
             case ZOOM:
               rsmview->GLZoom();
               Draw();
               break;
             case PAN:
               rsmview->GLPan();
               Draw();
               break;
             }

             rsmview->old_mp.x = rsmview->new_mp.x;
             rsmview->old_mp.y = rsmview->new_mp.y;
           }
  private: System::Void pictureBox1_MouseDown(System::Object* sender, System::Windows::Forms::MouseEventArgs*  e)
           {
             if (rsmview == NULL)  return;

             rsmview->first_mp.x = rsmview->old_mp.x = e->X;
             rsmview->first_mp.y = rsmview->old_mp.y = e->Y;

             rsmview->CalcCenter();

             int testval;
             switch (e->Button) {
             case MouseButtons::Left: // left
               testval = MK_LBUTTON;
               break;
             case MouseButtons::Middle: // middle
               testval = MK_MBUTTON;
               break;
             case MouseButtons::Right: // right
               testval = MK_RBUTTON;
               break;
             }
             if (mouseAssign->rot == testval) {
               if (shiftState) {
                 rsmview->opState = SCRN_ROT;
               } else {
                 rsmview->opState = OBJ_ROT;
               }
               rsmview->SearchRotOrigin();
             } else if (mouseAssign->zoom == testval) {
               rsmview->opState = ZOOM;
             } else if (mouseAssign->pan == testval) {
               rsmview->opState = PAN;
             }
           }
  private: System::Void pictureBox1_MouseUp(System::Object* sender, System::Windows::Forms::MouseEventArgs*  e)
           {
             if (rsmview == NULL)  return;

             rsmview->opState = NOSTATE;
             Draw();
           }
  private: System::Void treeView1_KeyDown(Object* sender, System::Windows::Forms::KeyEventArgs* e)
           {
             if (rsmview == NULL)  return;
             switch (e->KeyCode) {
             case Keys::Escape:
               menuExit_Click(NULL, NULL);
               break;
             case Keys::Z:
               rsmview->SetCameraDefault(true);
               Draw();
               break;
             case Keys::ShiftKey:
               shiftState = true;
               break;
             }
           }
  private: System::Void treeView1_KeyUp(Object* sender, System::Windows::Forms::KeyEventArgs* e)
           {
             if (e->KeyCode == Keys::ShiftKey)  shiftState = false;
           }
  private: System::Void treeView1_AfterSelect(System::Object* sender, System::Windows::Forms::TreeViewEventArgs* e)
           {
             TreeViewSelect(this->treeView1->SelectedNode->get_Handle().ToInt32());
           }
  private: System::Void menuDispChange_Click(System::Object *  sender, System::EventArgs *  e)
           {
             rsmview->dispmode++;
             if (rsmview->dispmode == 3)  rsmview->dispmode = 0;
             Draw();
           }        

  private: System::Void menuAddPak_Click(System::Object *  sender, System::EventArgs *  e)
           {
             if ((pak1->pakversion[0] & 0xFF00) == 0xFF00)  return;

             char cdir[MAX_PATH];
             GETCWD(cdir, sizeof(cdir));
             this->openFileDialog1->set_Filter(S"sdata.grf file (*.grf)|*.grf|all files (*.*)|*.*" );
             if (this->openFileDialog1->ShowDialog() == DialogResult::OK) {
               CHDIR(cdir);
               pak1->AddPakFile((char *)Marshal::StringToHGlobalAnsi(this->openFileDialog1->FileName).ToPointer());
               treeView1->Nodes->Clear();
               BuildTree();
               OGLResize();
             } else {
               CHDIR(cdir);
             }
           }
  private: System::Void menuViewReset_Click(System::Object *  sender, System::EventArgs *  e)
           {
             rsmview->SetCameraDefault(true);
             Draw();
           }

  private: System::Void menuSaveAsX_Click(System::Object *  sender, System::EventArgs *  e)
           {
             if (rsmview == NULL) return;
             LOCALTREE *rt = NULL;
             if (TreeViewSelectCheck(this->treeView1->SelectedNode->get_Handle().ToInt32(), &rt) == false)  return;

             char cdir[MAX_PATH];
             GETCWD(cdir, sizeof(cdir));
             this->saveFileDialog1->set_Filter(S"DirectX X形式 (*.x)|*.x" );
             if (this->saveFileDialog1->ShowDialog() == DialogResult::OK) {
               CHDIR(cdir);
               rsmview->rsm->SaveModelAsX((char *)Marshal::StringToHGlobalAnsi(this->saveFileDialog1->FileName).ToPointer());
             }
             CHDIR(cdir);
           }

private: System::Void menuChangeBGCol_Click(System::Object *  sender, System::EventArgs *  e)
         {
           if (rsmview == NULL)  return;
           if (rsmview->rsm == NULL)  return;

           if (this->colorDialog1->ShowDialog() == DialogResult::OK) {
             rsmview->bgcol.r = this->colorDialog1->get_Color().get_R() / (float)255;
             rsmview->bgcol.g = this->colorDialog1->get_Color().get_G() / (float)255;
             rsmview->bgcol.b = this->colorDialog1->get_Color().get_B() / (float)255;
             wglMakeCurrent(hdc, hglrc);
             rsmview->InitOpenGL();
             rsmview->Draw();
             SwapBuffers(hdc);
             wglMakeCurrent(hdc, 0);
           }
         }

private: System::Void menuChangeLineCol_Click(System::Object *  sender, System::EventArgs *  e)
         {
           if (rsmview == NULL)  return;

           if (this->colorDialog1->ShowDialog() == DialogResult::OK) {
             rsmview->rsm->linergb.r = this->colorDialog1->get_Color().get_R() / (float)255;
             rsmview->rsm->linergb.g = this->colorDialog1->get_Color().get_G() / (float)255;
             rsmview->rsm->linergb.b = this->colorDialog1->get_Color().get_B() / (float)255;
             Draw();
           }
         }

};
}


