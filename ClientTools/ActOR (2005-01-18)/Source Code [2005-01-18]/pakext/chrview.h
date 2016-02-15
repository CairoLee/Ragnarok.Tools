#ifndef __MY_CHRVIEW__
#define __MY_CHRVIEW__

#include <libintl.h>
#include <locale.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "../common/global.h"
#include "../common/pak.h"
#include "../common/Sprite.h"
#include "../common/vec.h"
#include "../common/codeconv.h"

#define GL_WIN_WIDTH 300
#define GL_WIN_HEIGHT 300

#define MAX_PC_PAT 9

typedef struct {
  char *filename;
  char *dispname;
} NAME_FvsD;

typedef struct {
  int num;
  NAME_FvsD *name;
} NFD;

#ifndef WIN32
typedef long LONG;

typedef struct {
  LONG x, y;
} POINT;
#endif

typedef struct {
  POINT *offs;
} POINTs;

typedef struct {
  char name[256];
  POINTs *ofs;
} HEADOffs;

class CChrView
{
private:
  CPAK *pak1;

  void InitNFDMob(char *fnp, NFD *nfd);
  void InitNFDPCBody(char *fnp, NFD *nfd, char *suffix);
  void InitNFDPCAcc(char *fnp, NFD *nfd);
  void InitNFDNPC(char *fnp, NFD *nfd);
  void InitNFDADD(char *dn, char *fn, NFD *nfd);
  void TrimDirSuffix(char *to, char *from, int leftlen);

  char *GetNFDFnameFromDname(NFD *nfd, char *dispname);
  void ChangeToSpr(char *fname, char *sprname);
  void GetItemNoToWeaponType(char *fname, int no);

public:
  CChrView(CPAK *pak);
  virtual ~CChrView();

  bool inMDragging;
  bool inRDragging;
  POINT first_mp, old_mp, new_mp;
  CAM_STATE camera;

  NFD NFDMob;
  NFD NFDPCBodyM;
  NFD NFDPCBodyF;
  NFD NFDPCAccM;
  NFD NFDPCAccF;
  NFD NFDPCWeapon;
  NFD NFDPCShield;
  NFD NFDNPC;

  bool sprready;
  CSprite *sprite_Body;
  CSprite *sprite_Head;
  CSprite *sprite_Acc1;
  CSprite *sprite_Acc2;
  CSprite *sprite_Acc3;
  CSprite *sprite_Weapon;
  CSprite *sprite_WpAftimg;
  CSprite *sprite_Shield;
  int curAct, curPat;
  int numAct, numPat;
  int curdirection;
  int curHead;
  int curAcc1, curAcc2, curAcc3;
  DWORD colBG;

  void DestroySpriteAll(void);
  void DestroySprite(CSprite **sprite);
  void DestroyNFD(NFD *nfd);
  void DestroyNFD_WeapShield(void);
  void ClearNFD(void);
  void InitNFD(void);

  void InitControlsWeaponShieldPre(void);

  void InitOpenGL();
  void SetCameraDefault(void);
  void CameraSet(void);
  void GLZoom(void);
  void GLPan(void);

  bool SpriteSetMob(CSprite **sprite, char *dispname);
  bool SpriteSetPCBody(CSprite **sprite, char *dispname, char *fname_base, bool male);
  void DestroySprites_GM2(void);
  bool SpriteSetPCHead(CSprite **sprite, bool male);
  bool SpriteSetPCAcc(CSprite **sprite, char *dispname, int no, bool male);
  bool SpriteSetPCWeapon(CSprite **sprite, char *dispname, char *bodyname, char *sexname);
  bool SpriteSetPCWeaponAftImage(char *fname, char *bodyname, char *sexname, int idWeapon);
  bool SpriteSetPCShield(CSprite **sprite, char *bodyname, char *sexname, char *dispname);
  bool SpriteSetNPC(CSprite **sprite, char *dispname);
  bool SpriteSet(CSprite **sprite, char *actname, char *sprname);
  void DrawSprites(void);
  void DrawSpritesHead(CSprite *sprite, int x, int y);

  void InitNumActPat(void);
  void SetNullSprites(void);

  void SelectAct(int idAct);
  void SelectDirection(int idAct, int idDirection);

  void SetWeaponName(char *fnp, char *bodynameW, char *bodyname2);
  void SetShieldName(char *fnp, char *bodynameS, char *bodyname2);

  void GetPCBodyName(char *bodyname, char *sexname, char *dispname, bool male);

  int GetTimeInterval(void);
};


extern "C" {
int NAME_FvsDSortFuncByDisp(const void *a, const void *b);
}

#define PC_ACTNAME_INT "PCActName%d"
#define MOB_ACTNAME_INT "MobActName%d"

#define MALE_STR "\263\262"
#define FEMALE_STR "\277\251"
#define AFTERIMAGE_STR "\260\313\261\244"
#define AFTIMG_ADD_STR "\137\260\313\261\244"
#define SHORTSWORD_AFTIMG_STR "\264\334\260\313\137\260\313\261\244"
#define SWORD_AFTIMG_STR "\260\313\137\260\313\261\244"
#define AXE_AFTIMG_STR "\265\265\263\242\137\260\313\261\244"
#define SPEAR_AFTIMG_STR "\303\242\137\260\313\261\244"
#define ROD_AFTIMG_STR "\267\324\265\345\137\260\313\261\244"
#define CRUB_AFTIMG_STR "\305\254\267\264\137\260\313\261\244"
#define CLAW_AFTIMG_STR "\263\312\305\254\137\260\313\261\244"

//#define MOB_DIR "data\\sprite\\ｸｺﾅﾍ\\"
#define MOB_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\270\363\275\272\305\315\134"
//#define PCBODY_M_DIR "data\sprite\ﾀﾎｰ｣ﾁｷ\ｸ・ｳｲ\"
#define PCBODY_M_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\300\316\260\243\301\267\134\270\366\305\353\134\263\262\134"
//#define PCBODY_F_DIR "data\sprite\ﾀﾎｰ｣ﾁｷ\ｸ・ｿｩ\"
#define PCBODY_F_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\300\316\260\243\301\267\134\270\366\305\353\134\277\251\134"
//#define PCHEAD_M_DIR "data\sprite\ﾀﾎｰ｣ﾁｷ\ｸﾓｸｮﾅ・ｳｲ\"
#define PCHEAD_M_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\300\316\260\243\301\267\134\270\323\270\256\305\353\134\263\262\134"
//#define PCHEAD_F_DIR "data\sprite\ﾀﾎｰ｣ﾁｷ\ｸﾓｸｮﾅ・ｿｩ\"
#define PCHEAD_F_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\300\316\260\243\301\267\134\270\323\270\256\305\353\134\277\251\134"
//#define PCACC_M_DIR "data\sprite\ｾﾇｼｼｻ邵ｮ\ｳｲ\"
#define PCACC_M_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\276\307\274\274\273\347\270\256\134\263\262\134"
//#define PCACC_F_DIR "data\sprite\ｾﾇｼｼｻ邵ｮ\ｿｩ\"
#define PCACC_F_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\276\307\274\274\273\347\270\256\134\277\251\134"
//#define WEAPON_DIR "data\sprite\ﾀﾎｰ｣ﾁｷ\"
#define WEAPON_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\300\316\260\243\301\267\134"
//#define SHIELD_DIR "data\sprite\ｹ貳ﾐ"\"
#define SHIELD_DIR "\144\141\164\141\134\163\160\162\151\164\145\134\271\346\306\320\134"
#define NPC_DIR "data\\sprite\\npc\\"

//#define BAPHOMET_PET "baphomet_ｻﾁｴﾙｱｸｸﾚ"
#define BAPHOMET_PET "\142\141\160\150\157\155\145\164\137\273\301\264\331\261\270\270\360\300\332"
//#define BON_GON_PET "bon_gun_ｿｵﾈｯｵｵｻ邁ﾋ"
#define BON_GON_PET "\142\157\156\137\147\165\156\137\277\265\310\257\265\265\273\347\260\313"
//#define CHOCHO_PET "chocho_ｹ豬ｶｸ・"
#define CHOCHO_PET "\143\150\157\143\150\157\137\271\346\265\266\270\351"
//#define DESERT_WOLF_B_PET "desert_wolf_b_ｿ・ﾖｺｹｸﾓｸｮ"
#define DESERT_WOLF_B_PET "\144\145\163\145\162\164\137\167\157\154\146\137\142\137\277\354\301\326\272\271\270\323\270\256"
//#define DEVIRUCHI_PET "deviruchi_ﾁ･ｲﾀﾁ・"
#define DEVIRUCHI_PET "\144\145\166\151\162\165\143\150\151\137\301\245\262\300\301\366"
//#define DOKEBI_PET "dokebi_ｾﾆﾈﾄｷﾎｸﾓｸｮ"
#define DOKEBI_PET "\144\157\153\145\142\151\137\276\306\310\304\267\316\270\323\270\256"
//#define ISIS_PET "isis_ﾅｬｷｹｿﾀﾆﾄﾆｮｶﾓｸｮｶ・"
#define ISIS_PET "\151\163\151\163\137\305\254\267\271\277\300\306\304\306\256\266\363\270\323\270\256\266\354"
//#define LUNATIC_PET "lunatic_ｸｮｺｻ"
#define LUNATIC_PET "\154\165\156\141\164\151\143\137\270\256\272\273"
//#define MUNAK_PET "munak_ｿ莨德ﾀ"
#define MUNAK_PET "\155\165\156\141\153\137\277\344\274\372\272\300"
//#define ORK_WARRIOR_PET "ork_warrior_ｲﾉ"
#define ORK_WARRIOR_PET "\157\162\153\137\167\141\162\162\151\157\162\137\262\311"
//#define PECOPECO_PET "pecopeco_ｳｿｺ・"
#define PECOPECO_PET "\160\145\143\157\160\145\143\157\137\263\277\272\361"
//#define PETIT_PET "petit_ｺｰ"
#define PETIT_PET "\160\145\164\151\164\137\272\260"
//#define PICKEY_PET "picky_ｾﾋｲｮﾁ・"
#define PICKY_PET "\160\151\143\153\171\137\276\313\262\256\301\372"
//#define PORING_PET "poring_ﾃ･ｰ｡ｹ・"
#define PORING_PET "\160\157\162\151\156\147\137\303\245\260\241\271\346"
//#define ROCKER_PET "rocker_ｸﾞｶﾑｱ篝ﾈｰ・"
#define ROCKER_PET "\162\157\143\153\145\162\137\270\336\266\321\261\342\276\310\260\346"
//#define SAVAGE_BABE_PET "savage_babe_ｷｹﾀﾌｽｺ"
#define SAVAGE_BABE_PET "\163\141\166\141\147\145\137\142\141\142\145\137\267\271\300\314\275\272"
//#define SMOKIE_PET "smokie_ｸﾓﾇﾃｷｯ"
#define SMOKIE_PET "\163\155\157\153\151\145\137\270\323\307\303\267\257"
//#define SOHEE_PET "sohee_ｹ豼・"
#define SOHEE_PET "\163\157\150\145\145\137\271\346\277\357"
//#define SPORE_PET "spore_ｿﾖｹﾎﾄ｡ｸｶ"
#define SPORE_PET "\163\160\157\162\145\137\277\370\301\326\271\316\304\241\270\266"
//#define YOYO_PET "yoyo_ｸﾓｸｮｶ・"
#define YOYO_PET "\171\157\171\157\137\270\323\270\256\266\354"

static int weaponid2type[][2] = {
  // 0: short sword
  // 1: one hand sword
  // 2: two hand sword
  // 3: axe
  // 4: spear
  // 5: rod
  // 6: crub
  // 7: book
  // 8: claw
  // 9: inst
  // 10: whip
  // 11: katar?
  1101, 1,  //ソード
  1102, 1,  //ソード
  1103, 1,  //ソード
  1104, 1,  //ファルシオン
  1105, 1,  //ファルシオン
  1106, 1,  //ファルシオン
  1107, 1,  //ブレイド
  1108, 1,  //ブレイド
  1109, 1,  //ブレイド
  1110, 1,  //レイピア
  1111, 1,  //レイピア
  1112, 1,  //レイピア
  1113, 1,  //シミター
  1114, 1,  //シミター
  1115, 1,  //シミター
  1116, 2,  //カタナ
  1117, 2,  //カタナ
  1118, 2,  //カタナ
  1119, 1,  //ツルギ
  1120, 1,  //ツルギ
  1121, 1,  //ツルギ
  1122, 1,  //環頭太刀
  1123, 1,  //海東剣
  1124, 1,  //オーキッシュソード
  1125, 1,  //環頭太刀
  1126, 1,  //サーベル
  1127, 1,  //サーベル
  1128, 1,  //海東剣
  1129, 1,  //フランベルジェ
  1130, 1,  //ネイガン
  1131, 1,  //アイスファルシオン
  1132, 1,  //エッジ
  1133, 1,  //ファイアーブランド
  1134, 1,  //シーザーズソード
  1135, 1,  //カトラス
  1136, 1,  //太陽剣
  1137, 1,  //エクスキャリバー
  1138, 1,  //ミステールテイン
  1139, 1,  //テイルフィング
  1140, 1,  //別雲剣
  1141, 1,  //無形剣
  1142, 1,  //ジュエルソード
  1143, 1,  //ガイアソード
  1144, 1,  //刺身包丁
  1145, 1,  //ホーリーアヴェンジャー
  //1151, 2,  //スレイヤー
  //1152, 2,  //スレイヤー
  //1153, 2,  //スレイヤー
  //1154, 2,  //バスタードソード
  //1155, 2,  //バスタードソード
  //1156, 2,  //バスタードソード
  //1157, 2,  //ツーハンドソード
  //1158, 2,  //ツーハンドソード
  //1159, 2,  //ツーハンドソード
  //1160, 2,  //ブロードソード
  //1161, 2,  //バルムン
  //1162, 2,  //ブロードソード
  //1163, 2,  //クレイモア
  //1164, 2,  //村正
  //1165, 2,  //正宗
  //1166, 2,  //ドラゴンスレイヤー
  //1167, 2,  //シュバイチェルシャベル
  //1168, 2,  //ツヴァイハンダー
  //1169, 2,  //エクスキューショナー
  //1170, 2,  //カッツバルゲル
  1201, 0,  //ナイフ
  1202, 0,  //ナイフ
  1203, 0,  //ナイフ
  1204, 0,  //カッター
  1205, 0,  //カッター
  1206, 0,  //カッター
  1207, 0,  //マインゴーシュ
  1208, 0,  //マインゴーシュ
  1209, 0,  //マインゴーシュ
  1210, 0,  //ダーク
  1211, 0,  //ダーク
  1212, 0,  //ダーク
  1213, 0,  //ダガー
  1214, 0,  //ダガー
  1215, 0,  //ダガー
  1216, 0,  //スティレット
  1217, 0,  //スティレット
  1218, 0,  //スティレット
  1219, 0,  //グラディウス
  1220, 0,  //グラディウス
  1221, 0,  //グラディウス
  1222, 0,  //ダマスカス
  1223, 0,  //フォーチュンソード
  1224, 0,  //ソードブレイカー
  1225, 0,  //メイルブレイカー
  1226, 0,  //ダマスカス
  1227, 0,  //ウィーダーナイフ
  1228, 0,  //コンバットナイフ
  1229, 0,  //包丁
  1230, 0,  //錐
  1231, 0,  //バゼラルド
  1232, 0,  //アサシンダガー
  1233, 0,  //エキサーサイズ
  1234, 0,  //月光剣
  1235, 0,  //アゾート
  1236, 0,  //スクサマッド
  1237, 0,  //オーガトゥース
  1238, 0,  //ゼニーナイフ
  1239, 0,  //ポイズンナイフ
  1240, 0,  //プリンセスナイフ
  1241, 0,  //カースダガー
  1242, 0,  //カウンターダガー
  //1250, 11,  //ジュル
  //1251, 11,  //ジュル
  //1252, 11,  //カタール
  //1253, 11,  //カタール
  //1254, 11,  //ジャマダハル
  //1255, 11,  //ジャマダハル
  1301, 3,  //アックス
  1302, 3,  //アックス
  1303, 3,  //アックス
  1304, 3,  //オーキッシュアックス
  1305, 3,  //クリーヴァー
  1306, 3,  //ウォーアクス
  1351, 3,  //バトルアックス
  1352, 3,  //バトルアックス
  1353, 3,  //バトルアックス
  1354, 3,  //ハンマー
  1355, 3,  //ハンマー
  1356, 3,  //ハンマー
  1357, 3,  //バスター
  1358, 3,  //バスター
  1359, 3,  //バスター
  1360, 3,  //ツーハンドアックス
  1361, 3,  //ツーハンドアックス
  1362, 3,  //ツーハンドアックス
  1363, 3,  //ブラッドアックス
  1364, 3,  //グレイトアックス
  1365, 3,  //サバス
  1366, 3,  //ライトエプシロン
  1367, 3,  //スローター
  1368, 3,  //トマホーク
  1369, 3,  //ギロチン
  1401, 4,  //ジャベリン
  1402, 4,  //ジャベリン
  1403, 4,  //ジャベリン
  1404, 4,  //スピアー
  1405, 4,  //スピアー
  1406, 4,  //スピアー
  1407, 4,  //パイク
  1408, 4,  //パイク
  1409, 4,  //パイク
  1410, 4,  //ランス
  1411, 4,  //ランス
  1412, 4,  //ランス
  1413, 4,  //グングニール
  1414, 4,  //ゲラドリア
  1415, 4,  //串
  1416, 4,  //テュングレティー
  1417, 4,  //ポールアクス
  1451, 4,  //ギザルム
  1452, 4,  //ギザルム
  1453, 4,  //ギザルム
  1454, 4,  //グレイヴ
  1455, 4,  //グレイヴ
  1456, 4,  //グレイヴ
  1457, 4,  //パルチザン
  1458, 4,  //パルチザン
  1459, 4,  //パルチザン
  1460, 4,  //トライデント
  1461, 4,  //トライデント
  1462, 4,  //トライデント
  1463, 4,  //ハルバード
  1464, 4,  //ハルバード
  1465, 4,  //ハルバード
  1466, 4,  //クレセントサイダー
  1467, 4,  //ビルギザルム
  1468, 4,  //ゼピュロス
  1469, 4,  //ロンギヌスの槍
  1470, 4,  //ブリューナク
  1471, 4,  //ヘルファイア
  1472, 5,  //スタッフオブソウル
  1473, 5,  //ウィザードスタッフ
  1501, 6,  //クラブ
  1502, 6,  //クラブ
  1503, 6,  //クラブ
  1504, 6,  //メイス
  1505, 6,  //メイス
  1506, 6,  //メイス
  1507, 6,  //スマッシャー
  1508, 6,  //スマッシャー
  1509, 6,  //スマッシャー
  1510, 6,  //フレイル
  1511, 6,  //フレイル
  1512, 6,  //フレイル
  1513, 6,  //モニングスター
  1514, 6,  //モニングスター
  1515, 6,  //モニングスター
  1516, 6,  //ソードメイス
  1517, 6,  //ソードメイス
  1518, 6,  //ソードメイス
  1519, 6,  //チェイン
  1520, 6,  //チェイン
  1521, 6,  //チェイン
  1522, 6,  //スタナー
  1523, 6,  //スパイク
  1524, 6,  //ゴールデンメイス
  1525, 6,  //ロングメイス
  1526, 6,  //スラッシュ 
  1527, 6,  //カドリール
  1528, 6,  //グランドクロス
  1529, 6,  //アイアンドライバー
  1530, 6,  //ミョルニール
  1531, 6,  //スパナ
  //1550, ,  //ブック
  //1551, ,  //バイブル
  //1552, ,  //タブレット
  1601, 5,  //ロッド
  1602, 5,  //ロッド
  1603, 5,  //ロッド
  1604, 5,  //ワンド
  1605, 5,  //ワンド
  1606, 5,  //ワンド
  1607, 5,  //スタッフ
  1608, 5,  //スタッフ
  1609, 5,  //スタッフ
  1610, 5,  //アークワンド
  1611, 5,  //アークワンド
  1612, 5,  //アークワンド
  1613, 5,  //マイトスタッフ
  1614, 5,  //神官の杖
  1615, 5,  //骸骨の杖
  //1701, ,  //ボウ
  //1702, ,  //ボウ
  //1703, ,  //ボウ
  //1704, ,  //コンポジットボウ
  //1705, ,  //コンポジットボウ
  //1706, ,  //コンポジットボウ
  //1707, ,  //グレイトボウ
  //1708, ,  //グレイトボウ
  //1709, ,  //グレイトボウ
  //1710, ,  //クロスボウ
  //1711, ,  //クロスボウ
  //1712, ,  //クロスボウ
  //1713, ,  //アーバレスト
  //1714, ,  //角弓
  //1715, ,  //アーバレスト
  //1716, ,  //角弓
  //1718, ,  //ハンターボウ
  //1719, ,  //盗賊の弓
  //1720, ,  //ルドラの弓
  //1721, ,  //連弩
  //1722, ,  //バリスタ
  1801, 8,  //バグナウ
  1802, 8,  //バグナウ
  1803, 8,  //ナックルダスター
  1804, 8,  //ナックルダスター
  1805, 8,  //ホラ
  1806, 8,  //ホラ
  1807, 8,  //フィスト
  1808, 8,  //フィスト
  1809, 8,  //クロー
  1810, 8,  //クロー
  1811, 8,  //フィンガー
  1812, 8,  //フィンガー
  1813, 8,  //カイザーナックル
  1814, 8  //ベルセルク
  //1901, ,  //バイオリン
  //1902, ,  //バイオリン
  //1903, ,  //マンドリン
  //1904, ,  //マンドリン
  //1905, ,  //リュート
  //1906, ,  //リュート
  //1907, ,  //ギター
  //1908, ,  //ギター
  //1909, ,  //ハープ
  //1910, ,  //ハープ
  //1911, ,  //ゴムンゴ
  //1912, ,  //ゴムンゴ
  //1950, ,  //ロープ
  //1951, ,  //ロープ
  //1952, ,  //ライン
  //1953, ,  //ライン
  //1954, ,  //ワイヤー
  //1955, ,  //ワイヤー
  //1956, ,  //ランテ
  //1957, ,  //ランテ
  //1958, ,  //テイル
  //1959, ,  //テイル
  //1960, ,  //ホイップ
  //1961, ,  //ホイップ
  //1962, ,  //ラリエット
  //1963, ,  //ラプチャーローズ
  //1964, ,  //シュメティ
};

#endif // __MY_CHRVIEW__
