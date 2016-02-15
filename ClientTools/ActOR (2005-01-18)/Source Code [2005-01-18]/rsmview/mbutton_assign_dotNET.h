#pragma once

#include "../common/global.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

extern MOUSE_ASSIGN *mouseAssign;

namespace rsmview
{
	/// <summary> 
	/// mbutton_assign_dotNET の概要
	///
	/// 警告 : このクラスの名前を変更する場合、このクラスが依存するすべての .resx ファイルに関連付けられた 
	///          マネージ リソース コンパイラ ツールに対して 'Resource File Name' プロパティを 
	///          変更する必要があります。この変更を行わないと、
	///          デザイナと、このフォームに関連付けられたローカライズ済みリソースとが、
	///          正しく相互に利用できなくなります。
	/// </summary>
public __gc class mbutton_assign_dotNET : public System::Windows::Forms::Form
	{
  private:
    bool MACheckThrough;
    void SetMouseAssign(void);
    void SetRadioButton(void);
    void CheckRadioButtons(int id);

	public: 
		mbutton_assign_dotNET(void)
		{
      MACheckThrough = true;
			InitializeComponent();
      SetRadioButton();
      MACheckThrough = false;
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
  private: System::Windows::Forms::GroupBox *  groupBox1;
  private: System::Windows::Forms::GroupBox *  groupBox2;
  private: System::Windows::Forms::GroupBox *  groupBox3;
  private: System::Windows::Forms::RadioButton *  rb13;
  private: System::Windows::Forms::RadioButton *  rb12;
  private: System::Windows::Forms::RadioButton *  rb11;
  private: System::Windows::Forms::RadioButton *  rb23;
  private: System::Windows::Forms::RadioButton *  rb22;
  private: System::Windows::Forms::RadioButton *  rb21;
  private: System::Windows::Forms::RadioButton *  rb33;
  private: System::Windows::Forms::RadioButton *  rb32;
  private: System::Windows::Forms::RadioButton *  rb31;
  private: System::Windows::Forms::Label *  label1;
  private: System::Windows::Forms::Button *  btnOK;
  private: System::Windows::Forms::Button *  btnCancel;




	private:
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// デザイナ サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディタで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
      this->groupBox1 = new System::Windows::Forms::GroupBox();
      this->rb13 = new System::Windows::Forms::RadioButton();
      this->rb12 = new System::Windows::Forms::RadioButton();
      this->rb11 = new System::Windows::Forms::RadioButton();
      this->groupBox2 = new System::Windows::Forms::GroupBox();
      this->rb23 = new System::Windows::Forms::RadioButton();
      this->rb22 = new System::Windows::Forms::RadioButton();
      this->rb21 = new System::Windows::Forms::RadioButton();
      this->groupBox3 = new System::Windows::Forms::GroupBox();
      this->rb33 = new System::Windows::Forms::RadioButton();
      this->rb32 = new System::Windows::Forms::RadioButton();
      this->rb31 = new System::Windows::Forms::RadioButton();
      this->label1 = new System::Windows::Forms::Label();
      this->btnOK = new System::Windows::Forms::Button();
      this->btnCancel = new System::Windows::Forms::Button();
      this->groupBox1->SuspendLayout();
      this->groupBox2->SuspendLayout();
      this->groupBox3->SuspendLayout();
      this->SuspendLayout();
      // 
      // groupBox1
      // 
      this->groupBox1->Controls->Add(this->rb13);
      this->groupBox1->Controls->Add(this->rb12);
      this->groupBox1->Controls->Add(this->rb11);
      this->groupBox1->Location = System::Drawing::Point(8, 16);
      this->groupBox1->Name = S"groupBox1";
      this->groupBox1->Size = System::Drawing::Size(184, 40);
      this->groupBox1->TabIndex = 0;
      this->groupBox1->TabStop = false;
      this->groupBox1->Text = S"左ボタン";
      // 
      // rb13
      // 
      this->rb13->Location = System::Drawing::Point(128, 11);
      this->rb13->Name = S"rb13";
      this->rb13->Size = System::Drawing::Size(48, 24);
      this->rb13->TabIndex = 2;
      this->rb13->Text = S"拡大";
      this->rb13->CheckedChanged += new System::EventHandler(this, rb13_CheckedChanged);
      // 
      // rb12
      // 
      this->rb12->Checked = true;
      this->rb12->Location = System::Drawing::Point(72, 11);
      this->rb12->Name = S"rb12";
      this->rb12->Size = System::Drawing::Size(48, 24);
      this->rb12->TabIndex = 1;
      this->rb12->TabStop = true;
      this->rb12->Text = S"回転";
      this->rb12->CheckedChanged += new System::EventHandler(this, rb12_CheckedChanged);
      // 
      // rb11
      // 
      this->rb11->Location = System::Drawing::Point(16, 11);
      this->rb11->Name = S"rb11";
      this->rb11->Size = System::Drawing::Size(48, 24);
      this->rb11->TabIndex = 0;
      this->rb11->Text = S"移動";
      this->rb11->CheckedChanged += new System::EventHandler(this, rb11_CheckedChanged);
      // 
      // groupBox2
      // 
      this->groupBox2->Controls->Add(this->rb23);
      this->groupBox2->Controls->Add(this->rb22);
      this->groupBox2->Controls->Add(this->rb21);
      this->groupBox2->Location = System::Drawing::Point(8, 64);
      this->groupBox2->Name = S"groupBox2";
      this->groupBox2->Size = System::Drawing::Size(184, 40);
      this->groupBox2->TabIndex = 1;
      this->groupBox2->TabStop = false;
      this->groupBox2->Text = S"中ボタン";
      // 
      // rb23
      // 
      this->rb23->Checked = true;
      this->rb23->Location = System::Drawing::Point(128, 12);
      this->rb23->Name = S"rb23";
      this->rb23->Size = System::Drawing::Size(48, 24);
      this->rb23->TabIndex = 5;
      this->rb23->TabStop = true;
      this->rb23->Text = S"拡大";
      this->rb23->CheckedChanged += new System::EventHandler(this, rb23_CheckedChanged);
      // 
      // rb22
      // 
      this->rb22->Location = System::Drawing::Point(72, 12);
      this->rb22->Name = S"rb22";
      this->rb22->Size = System::Drawing::Size(48, 24);
      this->rb22->TabIndex = 4;
      this->rb22->Text = S"回転";
      this->rb22->CheckedChanged += new System::EventHandler(this, rb22_CheckedChanged);
      // 
      // rb21
      // 
      this->rb21->Location = System::Drawing::Point(16, 12);
      this->rb21->Name = S"rb21";
      this->rb21->Size = System::Drawing::Size(48, 24);
      this->rb21->TabIndex = 3;
      this->rb21->Text = S"移動";
      this->rb21->CheckedChanged += new System::EventHandler(this, rb21_CheckedChanged);
      // 
      // groupBox3
      // 
      this->groupBox3->Controls->Add(this->rb33);
      this->groupBox3->Controls->Add(this->rb32);
      this->groupBox3->Controls->Add(this->rb31);
      this->groupBox3->Location = System::Drawing::Point(8, 112);
      this->groupBox3->Name = S"groupBox3";
      this->groupBox3->Size = System::Drawing::Size(184, 40);
      this->groupBox3->TabIndex = 2;
      this->groupBox3->TabStop = false;
      this->groupBox3->Text = S"右ボタン";
      // 
      // rb33
      // 
      this->rb33->Location = System::Drawing::Point(128, 13);
      this->rb33->Name = S"rb33";
      this->rb33->Size = System::Drawing::Size(48, 24);
      this->rb33->TabIndex = 8;
      this->rb33->Text = S"拡大";
      this->rb33->CheckedChanged += new System::EventHandler(this, rb33_CheckedChanged);
      // 
      // rb32
      // 
      this->rb32->Location = System::Drawing::Point(72, 13);
      this->rb32->Name = S"rb32";
      this->rb32->Size = System::Drawing::Size(48, 24);
      this->rb32->TabIndex = 7;
      this->rb32->Text = S"回転";
      this->rb32->CheckedChanged += new System::EventHandler(this, rb32_CheckedChanged);
      // 
      // rb31
      // 
      this->rb31->Checked = true;
      this->rb31->Location = System::Drawing::Point(16, 13);
      this->rb31->Name = S"rb31";
      this->rb31->Size = System::Drawing::Size(48, 24);
      this->rb31->TabIndex = 6;
      this->rb31->TabStop = true;
      this->rb31->Text = S"移動";
      this->rb31->CheckedChanged += new System::EventHandler(this, rb31_CheckedChanged);
      // 
      // label1
      // 
      this->label1->Location = System::Drawing::Point(16, 160);
      this->label1->Name = S"label1";
      this->label1->Size = System::Drawing::Size(176, 23);
      this->label1->TabIndex = 3;
      this->label1->Text = S"Shiftを押しながら回転で2D回転";
      // 
      // btnOK
      // 
      this->btnOK->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btnOK->Location = System::Drawing::Point(216, 56);
      this->btnOK->Name = S"btnOK";
      this->btnOK->Size = System::Drawing::Size(64, 24);
      this->btnOK->TabIndex = 4;
      this->btnOK->Text = S"OK";
      // 
      // btnCancel
      // 
      this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
      this->btnCancel->Location = System::Drawing::Point(216, 96);
      this->btnCancel->Name = S"btnCancel";
      this->btnCancel->Size = System::Drawing::Size(64, 24);
      this->btnCancel->TabIndex = 5;
      this->btnCancel->Text = S"キャンセル";
      // 
      // mbutton_assign_dotNET
      // 
      this->AutoScaleBaseSize = System::Drawing::Size(5, 12);
      this->ClientSize = System::Drawing::Size(298, 189);
      this->Controls->Add(this->btnCancel);
      this->Controls->Add(this->btnOK);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->groupBox3);
      this->Controls->Add(this->groupBox2);
      this->Controls->Add(this->groupBox1);
      this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
      this->Name = S"mbutton_assign_dotNET";
      this->Text = S"マウスボタン割り当て変更";
      this->groupBox1->ResumeLayout(false);
      this->groupBox2->ResumeLayout(false);
      this->groupBox3->ResumeLayout(false);
      this->Closing += new System::ComponentModel::CancelEventHandler(this, MAFrm_Closing);
      this->ResumeLayout(false);

    }		
private: System::Void MAFrm_Closing(System::Object *sender, System::ComponentModel::CancelEventArgs *e)
         {
           SetMouseAssign();
         }
private: System::Void rb11_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(0);
         }

private: System::Void rb12_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(1);
         }

private: System::Void rb13_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(2);
         }

private: System::Void rb21_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(3);
         }

private: System::Void rb22_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(4);
         }

private: System::Void rb23_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(5);
         }

private: System::Void rb31_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(6);
         }

private: System::Void rb32_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(7);
         }

private: System::Void rb33_CheckedChanged(System::Object *  sender, System::EventArgs *  e)
         {
           CheckRadioButtons(8);
         }

};
}
