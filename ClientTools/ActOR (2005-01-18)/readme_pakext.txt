巷に転がってるdata.grfのextract、その他ができます。
基本的にWin2k/XP用ですが、他所のツールと違って98/Meユーザーに易しいです。
（コピーライトはこのファイルの下の方）

DOSプロンプト、コマンドプロンプトから
pakext -h　で、ずら～っと使い方が表示されます。
フィーリングで使ってください

使い方例（あくまでも例です）

０．イージーモード
　pakext -E data.grf
　まず、ツリーの中で対象を選択してからボタンを押してください。
　起動が遅い場合は、
  　pakext -m .spr data.grf
　として、絞り込みましょう

１．リストの作成
　pakext -t data.grf

２．全展開（★98/Meはダメ）
　pakext data.grf

３．部分展開（★98/Meはダメ）
　　　pakext -m sprite data.grf
とか　pakext -m .txt data.grf

４．カレントのdataフォルダ以下のパッチ作成（要 -k付きでの展開）（★98/Meはダメ）
　pakext -p (or -P) -k

５．パッチ当て（※コンパイル済みexeからは削除されてます）
（それなりに安定してると思います）
　pakext -a update.gpf data.grf

６．キャラクタビューワー
  pakext -C data.grf
 や、pakext -C data.grf update.gpf
  マウス中ドラッグでズーム
  右ドラッグで移動

７．部分削除（旧形式はうまく動いてません。要バックアップ）
（※コンパイル済みexeからは削除されてます）
　pakext -s sdata.grf data.grf
 とすると、sdata.grf内のあるデータがdata.grf内の同一のものならば削除します。
　（sdata.grf - data.grf → 一致したデータを抜いたsdata.grf）
  pakext2 -S data.grf update.gpf
 とすると、data.grf内のあるデータがupdate.gpf内にあるファイル名が同一のものなら削除します。
　（data.grf - update.gpf → 一致したファイル名のデータを抜いたdata.grf）
　-sの使い道は見てそのままですが、-Sの使い道はちょっと考えて見ましょう。
　便利ですよね？ね？


Win98/MEでパッチ作りたいとき（Win 2k/XPでも可）
（※コンパイル済みexeからは削除されてます）
注：途中出てくるlist.txtを編集してセーブしないでください
　　理由は、「・」の文字コードが変換される恐れがあるからです

Step1 リストファイルの作成
 DOSプロンプトで
　pakext2 -t -k data.grf > list.txt
 として、リストファイルlist.txtを作成します。

Step2 対応ファイルの作成
 別のテキストファイルを以下の形式で作成します。

(数字)(タブ)(自分で用意したファイル名)

例. (同梱example_vs.txtも参考にしてください）
5088	c:\tmp\a.spr
3	a.act

 ここにある数字は、先に作ったリストファイルに書かれている数字です。
この数字をキーとして、対応ファイルに書かれたファイル名に対応させるわけです。
韓国語が読める人は、Step1でhangle.htmlというファイルも同時に
作成しているので（-t時に -kを使うとこうなる）、こっちを参考にして数字を
探してください。
 ハングルが読めない場合は、sjis-euckr-unicode.html (追記４)を参考にしてください

 ここで作成した対応ファイルのファイル名を仮にvs.txtとします。

Step3 起動
 DOSプロンプトから
 pakext -9
です

Step4 ファイルのセット
 リストファイル(list.txt)、対応ファイル(vs.txt)のそれぞれを参照ボタンを
押してセットしてください

Step5 確認
 対応の確認ボタンを押して、ファイル名の対応が取れているか確認してください
(自分で用意したファイル名)
 -> (data.grf内のファイル名）
と表示されるはずです

Step6 作成
 パッチのバージョンを選択して、update.gpf作成ボタンを押して作成してください
多分、カレントディレクトリにupdate.gpfが作成されます

手順が面倒ですが、すべてはSJISのコードを作って、標準にした会社のせいです。
ウィンドウがうざい場合は
pakext -9 [-p or -P] -l リストファイル名 -T 対応ファイル名 
で、コマンドプロンプト上で終了します。
（-p でver.2形式 -Pでver.1形式。何も付けないとver.1形式）
（※コンパイル済みexeからは削除されてます）




COPYRIGHT

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

追記：
　grfの展開処理にAthenaのコードを一部使用しています

追記２：
　－

追記３：
　リコンパイルは、winはVC++で、linuxはgcc/g++で。
　linuxではgtk+2.2を使ってます。ほかにgtkglextも必要。
  windowsでの再コンパイルは、
　  ○pakext-gtk、actor-gtkプロジェクトを削除する
  または、
　  ○http://gladewin32.sourceforge.net/のall in one installerをインストール後
　　  VC (or VS)の設定
  をする必要があります

追記４：
　同梱 share\locale\ja\LC_MESSAGES\grfviewer.l.moを適当に並べたもので、未使用のものも混ざってます

追記５：
　　和訳が気に入らない場合は、share\locale\ja\LC_MESSAGES\grfviewer.l.mo
　のファイルを　msgunfmtして、何かしらの処理をしてください。
　（だったら、.poを付けろって話もありますが…。アイテム名の出所が…）


windowsでの再コンパイルは、
　○pakext-gtk、actor-gtk、rsmview-gtkプロジェクトを削除する
または、
　○http://gladewin32.sourceforge.net/のall in one installerをインストール後
　　VC (or VS)の設定
    libintl.h内 標準のfprintfを使用するように変更
    zconf.h内 HAVE_UNISTD_Hの定義が間違っているのを修正
    
をする必要があります
