#include <math.h>
#include "vec.h"

#ifndef M_PI
#define M_PI 3.1415926536f
#endif

Vec::Vec()
{}

Vec::Vec(float a, float b, float c)
{
  x = a; y = b; z = c;
}

Vec::Vec(const Vec& v)
{
  x = v.x; y = v.y; z = v.z;
}

Vec& Vec::operator =(const Vec& v) {
	x = v.x; y = v.y; z = v.z;
	return *this;
}

Vec operator +(const Vec& a, const Vec& b) {
	Vec		c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	return c;
}

Vec operator -(const Vec& a, const Vec& b) {
	Vec		c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	return c;
}

double operator *(const Vec& a, const Vec& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec operator *(const Vec& a, float b) {
	Vec		c;
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
	return c;
}

Vec operator *(float b, const Vec& a) {
	Vec		c;
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
	return c;
}

Vec operator /(const Vec& a, float b) {
	Vec		c;
	c.x = a.x / b;
	c.y = a.y / b;
	c.z = a.z / b;
	return c;
}

Vec operator ^(const Vec& a, const Vec& b) {
	Vec		c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	return c;
}

float Vec::magnitude(void)
{
  return sqrt(x*x+y*y+z*z);
}

void Vec::normalize()
{
  float d = magnitude();
  x /= d; y /= d; z /= d;
}

void Vec::ThisAxisRotOffNorm(Vec *p, float degree, Vec *offs)
{
  Vec tmp[2];

  tmp[0] = (-1) * *offs;
  tmp[1] = *p - *offs;
  this->ThisAxisRot(&tmp[0], degree);
  this->ThisAxisRot(&tmp[1], degree);
  *p = tmp[1] - tmp[0];
  p->normalize();
}

void Vec::ThisAxisRot(Vec *p, float degree)
{
  float rad = degree / 180.f * M_PI;
  Vec ret;
  float cosr = (float)cos(rad);
  float sinr = (float)sin(rad);

  ret.x =  ( x * x * (1-cosr) + cosr ) * p->x;
  ret.x += ( x * y * (1-cosr) - z * sinr ) * p->y;
  ret.x += ( z * x * (1-cosr) + y * sinr ) * p->z;

  ret.y =  ( x * y * (1-cosr) + z *sinr ) * p->x;
  ret.y += ( y * y * (1-cosr) + cosr ) * p->y;
  ret.y += ( y * z * (1-cosr) - x * sinr) * p->z;

  ret.z =  ( z * x * (1-cosr) - y * sinr ) * p->x;
  ret.z += ( y * z * (1-cosr) + x * sinr ) * p->y;
  ret.z += ( z * z * (1-cosr) + cosr ) * p->z;

  *p = ret;
}

float Vec::CalcDistance2(Vec a)
{
  return (a.x - x)*(a.x - x) + (a.y - y)*(a.y - y) + (a.z - z)*(a.z - z);
}

bool Vec::CheckInTriangle(Vec a, Vec b, Vec c)
{
  double angle_sum = 0;
  Vec sa, sb, sc;

  sa = a - *this;
  sb = b - *this;
  sc = c - *this;

  angle_sum  = acos( sa*sb / sa.magnitude() / sb.magnitude() );
  angle_sum += acos( sb*sc / sb.magnitude() / sc.magnitude() );
  angle_sum += acos( sc*sa / sc.magnitude() / sa.magnitude() );
  angle_sum = fabs(angle_sum);

  if ((2*M_PI - 1.E-6 < angle_sum) && (angle_sum < 2*M_PI+1.E-6)) {
    return true;
  }

  return false;
}

Vec Vec::CalcPlaneNorm(Vec a, Vec b)
{
  Vec ret;

  ret = (a - *this) ^ (b - *this);

  return ret;
}


Vec Vec::CalcIntersectPoint(Vec norm, Vec p, Vec v)
{
  Vec retp;
  double t;

  t = ((p - *this)*norm) / (v*norm);
  retp = v * (float)t + *this;

  return retp;
}

float Vec::CalcLineParam(Vec dir, Vec a, Vec b, Vec c)
{
  Vec g;
  float ret;

  g = (a + b + c) / 3.f;

  ret = dir.x * (g.x - this->x) + dir.y * (g.y - this->y) + dir.z * (g.z - this->z);
  ret /= dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;

  return ret;

}
