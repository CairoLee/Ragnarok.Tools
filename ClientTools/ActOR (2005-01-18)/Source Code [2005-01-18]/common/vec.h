#ifndef ___AAA_Vector___
#define ___AAA_Vector___

class Vec {
public:
  float  x, y, z;
  Vec();
  Vec(float a, float b, float c);
  Vec(const Vec& v);
  Vec& operator =(const Vec& v);
  friend Vec operator +(const Vec& a, const Vec& b);
  friend Vec operator -(const Vec& a, const Vec& b);
  friend Vec operator *(const Vec& a, float b);
  friend Vec operator *(float b, const Vec& a);
  friend Vec operator /(const Vec& a, float b);
  // 内積
  friend double operator *(const Vec& a, const Vec& b);
  // 外積
  friend Vec operator ^(const Vec& a, const Vec& b);
  // スカラーを返す
  float magnitude(void);
  // 正規化
  void normalize(void);
  // このベクトル周りに回転させた点を返す(オフセット付）
  void ThisAxisRotOffNorm(Vec *p, float degree, Vec *offs);
  // このベクトル周りに回転させた点を返す
  void ThisAxisRot(Vec *p, float degree);
  // 距離の２乗を返す
  float CalcDistance2(Vec a);
  // この点が三角形abcの中にあるか調べる（この点は平面に乗ってるものとする）
  bool CheckInTriangle(Vec a, Vec b, Vec c);
  // この点とa, bで作られる平面の法線ベクトルを返す
  Vec CalcPlaneNorm(Vec a, Vec b);
  // 点pを通る法線ベクトルnormの平面とこの点を通るベクトルvの交点を求める
  Vec CalcIntersectPoint(Vec norm, Vec p, Vec v);
  // この点を通るベクトルdirと三角形ABCの偽重心との最短距離にある点でのパラメータを求める
  float CalcLineParam(Vec dir, Vec a, Vec b, Vec c);
};

#endif
