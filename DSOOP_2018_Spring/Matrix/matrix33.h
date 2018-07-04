#ifndef _MATRIX33_H_
#define _MATRIX33_H_
#include "vector3.h"

class matrix33
{
public:
  matrix33();
  matrix33(vector3 &cola, vector3 &colb, vector3 &colc);
  matrix33(const matrix33 &m);

  vector3 &operator[](unsigned int index);
  const vector3 &operator[](unsigned int index) const;

  matrix33 &operator=(const matrix33 &m);
  matrix33 &operator+=(const matrix33 &m);
  matrix33 &operator-=(const matrix33 &m);
  matrix33 &operator*=(float f);
  matrix33 &operator/=(float f);
  friend bool operator==(const matrix33 &a, const matrix33 &b);
  friend bool operator!=(const matrix33 &a, const matrix33 &b);
  friend matrix33 operator-(const matrix33 &m);
  friend matrix33 operator+(const matrix33 &a, const matrix33 &b);
  friend matrix33 operator-(const matrix33 &a, const matrix33 &b);
  friend matrix33 operator*(const matrix33 &m, float f);
  friend matrix33 operator*(float f, const matrix33 &m);
  friend matrix33 operator*(const matrix33 &a, const matrix33 &b);
  friend matrix33 operator/(const matrix33 &m, float f);

  void printMatrix() const;
  float determinant() const;
  matrix33 invert();
  void identity();

public:
  vector3 col1;
  vector3 col2;
  vector3 col3;
};

#endif