#ifndef _VECTOR3_H_
#define _VECTOR3_H_

class vector3
{
  public:
	/* data members */
	float x, y, z;

	//****************************************************//
	//  You need to implement the following functions !!  //
	//****************************************************//

  public:
	/* constructors */
	// default constructor -- don't do anything with it
	vector3();

	// constructor with initializing float values
	vector3(float inX, float inY, float inZ);

	// constructor with initializing vector3
	vector3(const vector3 &v);

  public:
	/* Operators */

	// access element
	float &operator[](unsigned int index);
	const float &operator[](unsigned int index) const;

	vector3 &operator=(const vector3 &v);
	vector3 &operator+=(const vector3 &v);
	vector3 &operator-=(const vector3 &v);
	vector3 &operator*=(float f);
	vector3 &operator/=(float f);
	friend bool operator==(const vector3 &a, const vector3 &b);
	friend bool operator!=(const vector3 &a, const vector3 &b);
	friend vector3 operator-(const vector3 &a);
	friend vector3 operator+(const vector3 &a, const vector3 &b);
	friend vector3 operator-(const vector3 &a, const vector3 &b);
	friend vector3 operator*(const vector3 &v, float f);
	friend vector3 operator*(float f, const vector3 &v);
	friend vector3 operator/(const vector3 &v, float f);

  public:
	/* Methods */

	// set values (e.g. x = xIn, ...)
	void set(float xIn, float yIn, float zIn);

	// calculate length
	float length() const;

	// print matrix using std::cout
	void printVector3() const;
};

#endif