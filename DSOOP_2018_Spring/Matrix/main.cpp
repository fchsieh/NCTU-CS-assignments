#include <iostream>

#include "matrix33.h"

using namespace std;

int main()
{
	// test vector class;
	cout << "VECTOR TEST 1:" << endl;
	vector3 vec1(1, 1, 3);
	vec1.printVector3();
	cout << endl;

	// correct answer: (5, 5, 15)
	cout << "VECTOR TEST 2:" << endl;
	vector3 vec2 = 5*vec1 + vec1 - vec1;
	vec2.printVector3();
	cout << endl;

	// correct answer: (5, 105, 19)
	cout << "VECTOR TEST 3:" << endl;
	vector3 vec3(vec2);
	vec3 += vector3(0, 100, 4);
	vec3.printVector3();
	cout << endl;

	// correct answer: (3, 3, 12)
	cout << "VECTOR TEST 4:" << endl;
	vector3 vec4;
	vec4 = vec1 + vector3(0, 0, 1);
	vec4 *= 3;
	vec4.printVector3();
	cout << endl;

	// correct answer: L2-norm of vec5 = 4.24264
	cout << "VECTOR TEST 5:" << endl;
	vector3 vec5(1, 1, 4);
	if(vec4/3 == vec5)
		cout << "vec4/3 == vec5" << endl;
	else
		cout << "your program is wrong :(" << endl;
	cout << "L2-norm of vec5 = " << vec5.length() << endl;
	cout << endl;

	// test matrix class
	// correct answer: 1 2 3
	//                 3 5 7
	//                 4 2 1
	cout << "MATRIX TEST 1:" << endl;
	vector3 vec6(1, 3, 4);
	vector3 vec7(2, 5, 2);
	vector3 vec8(3, 7, 1);
	matrix33 mat1(vec6, vec7, vec8);
	mat1.printMatrix();
	cout << endl;

	// correct answer: 1 3 1
	//                 8 2 3
	//                 4 5 6
	cout << "MATRIX TEST 2:" << endl;
	matrix33 mat2(mat1);
	mat2[0][0] = 1;
	mat2[0][1] = 8;
	mat2[0][2] = 4;
	mat2[1][0] = 3;
	mat2[1][1] = 2;
	mat2[1][2] = 5;
	mat2[2][0] = 1;
	mat2[2][1] = 3;
	mat2[2][2] = 6;
	mat2.printMatrix();
	cout << endl;

	// correct answer: -19  24  3
	//                  55 -63 -4
	//                 -30  35 -2
	cout << "MATRIX TEST 3:" << endl;
	matrix33 mat3 = mat1.invert();
	(mat3 * mat2).printMatrix();
	cout << endl;

	// correct answer: 7.5     0    0
	//                   0   7.5    0
	//                   0     0  7.5
	cout << "MATRIX TEST 4:" << endl;
	matrix33 mat4;
	mat4.identity();
	mat4 *= 5;
	matrix33 mat5 = mat4 + mat4*0.5;
	mat5.printMatrix();
	cout << endl;

	cout << "MATRIX TEST 5:" << endl;
	if(mat4 != mat5)
		cout << "mat4 != mat5" << endl;
	else 
		cout << "your program is wrong :(" << endl;
	cout << endl;

	// correct answer: det(mat4) = 125
	cout << "MATRIX TEST 6:" << endl;
	cout << "det(mat4)= " << mat4.determinant() << endl;
	cout << endl;

	return 0;
}