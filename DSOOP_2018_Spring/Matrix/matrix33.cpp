#include <iostream>
#include <stdexcept>
#include "matrix33.h"
using namespace std;

matrix33::matrix33()
{
}

matrix33::matrix33(vector3 &cola, vector3 &colb, vector3 &colc)
    : col1(cola),
      col2(colb),
      col3(colc)
{
}

matrix33::matrix33(const matrix33 &m)
{
    col1 = m.col1;
    col2 = m.col2;
    col3 = m.col3;
}

vector3 &matrix33::operator[](unsigned int index)
{
    if (index < 0 || index > 3)
        throw out_of_range("Index out of range\n");
    switch (index)
    {
    case 0:
        return col1;
        break;
    case 1:
        return col2;
        break;
    case 2:
        return col3;
        break;
    }
}

const vector3 &matrix33::operator[](unsigned int index) const
{
    if (index < 0 || index > 3)
        throw out_of_range("Index out of range\n");
    switch (index)
    {
    case 0:
        return col1;
        break;
    case 1:
        return col2;
        break;
    case 2:
        return col3;
        break;
    }
}

matrix33 &matrix33::operator=(const matrix33 &m)
{
    if (&m != this)
    {
        col1 = m.col1;
        col2 = m.col2;
        col3 = m.col3;
    }
    return *this;
}

matrix33 &matrix33::operator+=(const matrix33 &m)
{
    col1 += m.col1;
    col2 += m.col2;
    col3 += m.col3;
    return *this;
}

matrix33 &matrix33::operator-=(const matrix33 &m)
{
    col1 -= m.col1;
    col2 -= m.col2;
    col3 -= m.col3;
    return *this;
}

matrix33 &matrix33::operator*=(float f)
{
    col1 *= f;
    col2 *= f;
    col3 *= f;
    return *this;
}

matrix33 &matrix33::operator/=(float f)
{
    col1 /= f;
    col2 /= f;
    col3 /= f;
    return *this;
}

bool operator==(const matrix33 &a, const matrix33 &b)
{
    if (a.col1 == b.col1 && a.col2 == b.col2 && a.col3 == b.col3)
        return true;
    else
        return false;
}

bool operator!=(const matrix33 &a, const matrix33 &b)
{
    if (a.col1 == b.col1 || a.col2 == b.col2 || a.col3 == b.col3)
        return false;
    else
        return true;
}

matrix33 operator-(matrix33 &m)
{
    matrix33 temp;
    temp.col1 = -(m.col1);
    temp.col2 = -(m.col2);
    temp.col3 = -(m.col3);
    return temp;
}

matrix33 operator+(const matrix33 &a, const matrix33 &b)
{
    matrix33 temp;
    temp.col1 = a.col1 + b.col1;
    temp.col2 = a.col2 + b.col2;
    temp.col3 = a.col3 + b.col3;
    return temp;
}

matrix33 operator-(const matrix33 &a, const matrix33 &b)
{
    matrix33 temp;
    temp.col1 = a.col1 - b.col1;
    temp.col2 = a.col2 - b.col2;
    temp.col3 = a.col3 - b.col3;
    return temp;
}

matrix33 operator*(const matrix33 &m, float f)
{
    matrix33 temp;
    temp.col1 = m.col1 * f;
    temp.col2 = m.col2 * f;
    temp.col3 = m.col3 * f;
    return temp;
}

matrix33 operator*(const matrix33 &a, const matrix33 &b)
{
    matrix33 temp;
    temp[0][0] = a[0][0] * b[0][0] + a[1][0] * b[0][1] + a[2][0] * b[0][2];
    temp[1][0] = a[0][0] * b[1][0] + a[1][0] * b[1][1] + a[2][0] * b[1][2];
    temp[2][0] = a[0][0] * b[2][0] + a[1][0] * b[2][1] + a[2][0] * b[2][2];

    temp[0][1] = a[0][1] * b[0][0] + a[1][1] * b[0][1] + a[2][1] * b[0][2];
    temp[1][1] = a[0][1] * b[1][0] + a[1][1] * b[1][1] + a[2][1] * b[1][2];
    temp[2][1] = a[0][1] * b[2][0] + a[1][1] * b[2][1] + a[2][1] * b[2][2];

    temp[0][2] = a[0][2] * b[0][0] + a[1][2] * b[0][1] + a[2][2] * b[0][2];
    temp[1][2] = a[0][2] * b[1][0] + a[1][2] * b[1][1] + a[2][2] * b[1][2];
    temp[2][2] = a[0][2] * b[2][0] + a[1][2] * b[2][1] + a[2][2] * b[2][2];
    return temp;
}

matrix33 operator*(float f, const matrix33 &m)
{
    matrix33 temp;
    temp.col1 = m.col1 * f;
    temp.col2 = m.col2 * f;
    temp.col3 = m.col3 * f;
    return temp;
}

matrix33 operator/(const matrix33 &m, float f)
{
    matrix33 temp;
    temp.col1 = m.col1 / f;
    temp.col2 = m.col2 / f;
    temp.col3 = m.col3 / f;
    return temp;
}

void matrix33::printMatrix() const
{
    for (int i = 0; i < 3; i++)
        cout << col1[i] << " " << col2[i] << " " << col3[i] << "\n";
}

float matrix33::determinant() const
{
    return (col1[0] * (col2[1] * col3[2] - col3[1] * col2[2]) - col2[0] * (col1[1] * col3[2] - col3[1] * col1[2]) + col3[0] * (col1[1] * col2[2] - col2[1] * col1[2]));
}

matrix33 matrix33::invert()
{
    vector3 temp1 = col1;
    vector3 temp2 = col2;
    vector3 temp3 = col3;

    if (determinant() == 0)
        throw logic_error("Determinant is 0, an error occured.\n");

    // transpose the origin matrix
    col1[1] = temp2[0];
    col1[2] = temp3[0];
    col2[0] = temp1[1];
    col2[2] = temp3[1];
    col3[0] = temp1[2];
    col3[1] = temp2[2];

    float a0, a1, a2; // inverse matrix column1
    float b0, b1, b2; // inverse matrix column2
    float c0, c1, c2; // inverse matrix column3

    a0 = (col2[1] * col3[2] - col3[1] * col2[2]) / determinant();
    a1 = (col2[0] * col3[2] - col3[0] * col2[2]) / determinant();
    a2 = (col2[0] * col3[1] - col3[0] * col2[1]) / determinant();
    b0 = (col1[1] * col3[2] - col3[1] * col1[2]) / determinant();
    b1 = (col1[0] * col3[2] - col3[0] * col1[2]) / determinant();
    b2 = (col1[0] * col3[1] - col3[0] * col1[1]) / determinant();
    c0 = (col1[1] * col2[2] - col2[1] * col1[2]) / determinant();
    c1 = (col1[0] * col2[2] - col2[0] * col1[2]) / determinant();
    c2 = (col1[0] * col2[1] - col2[0] * col1[1]) / determinant();

    a1 = (a1 == 0) ? 0 : -1 * a1;
    b0 = (b0 == 0) ? 0 : -1 * b0;
    b2 = (b2 == 0) ? 0 : -1 * b2;
    c1 = (c1 == 0) ? 0 : -1 * c1;

    vector3 tempV1(a0, a1, a2);
    vector3 tempV2(b0, b1, b2);
    vector3 tempV3(c0, c1, c2);

    matrix33 temp33(tempV1, tempV2, tempV3);
    col1 = temp1;
    col2 = temp2;
    col3 = temp3;
    return temp33;
}

void matrix33::identity()
{
    vector3 a1(1, 0, 0);
    vector3 a2(0, 1, 0);
    vector3 a3(0, 0, 1);
    col1 = a1;
    col2 = a2;
    col3 = a3;
}
