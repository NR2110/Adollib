//#include "complex.h"
//
//using namespace Adollib;
//
//#include <algorithm>
//
//#pragma region complex
//
//complex complex::operator+() const{
//	return complex(x, y);
//}
//
//complex complex::operator-() const {
//	return complex(-x, -y);
//}
//
//complex complex::operator+(const complex& C) const {
//	return complex(x + C.x, y + C.y);
//}
//
//complex complex::operator-(const complex& C) const {
//	return complex(x - C.x, y - C.y);
//}
//
//complex complex::operator*(const complex& C)const {
//	complex ret;
//	ret.x = x * C.x - y * C.y;
//	ret.y = x * C.y + y * C.x;
//
//	return ret;
//}
//
//complex complex::operator*(const float C)const {
//	complex ret;
//	ret.x = x * C - y * C;
//	ret.y = x * C + y * C;
//
//	return ret;
//}
//
//
//
//
//
//complex& complex::operator=(const complex& C) {
//	x = C.x;
//	y = C.y;
//	return *this;
//}
//
//complex& complex::operator+=(const complex& C) {
//	x += C.x;
//	y += C.y;
//	return *this;
//}
//
//complex& complex::operator-=(const complex& C) {
//	x -= C.x;
//	y -= C.y;
//	return *this;
//}
//
//complex& complex::operator*=(const complex& C) {
//
//	complex ret;
//	ret.x = x * C.x - y * C.y;
//	ret.y = x * C.y + y * C.x;
//
//	x = ret.x;
//	y = ret.y;
//
//	return *this;
//
//}
//
//complex& complex::operator*=(const float C) {
//	complex ret;
//	ret.x = x * C - y * C;
//	ret.y = x * C + y * C;
//
//	x = ret.x;
//	y = ret.y;
//
//	return *this;
//}
//
//bool complex::operator==(const complex& C) const{
//	return x == C.x && y == C.y;
//}
//
//bool complex::operator!=(const complex& C) const {
//	return x != C.x || y != C.y;
//}
//
//#pragma endregion
//
//#pragma region complex_matrix
//
//complex_matrix4x4& complex_matrix4x4::operator=(const complex_matrix4x4& C) {
//
//	_11 = C._11; _12 = C._12; _13 = C._13; _14 = C._14;
//	_21 = C._21; _22 = C._22; _23 = C._23; _24 = C._24;
//	_31 = C._31; _32 = C._32; _33 = C._33; _34 = C._34;
//	_41 = C._41; _42 = C._42; _43 = C._43; _44 = C._44;
//
//	return *this;
//}
//
//complex_matrix4x4& complex_matrix4x4::operator+=(const complex_matrix4x4& C) {
//
//	_11 += C._11; _12 += C._12; _13 += C._13; _14 += C._14;
//	_21 += C._21; _22 += C._22; _23 += C._23; _24 += C._24;
//	_31 += C._31; _32 += C._32; _33 += C._33; _34 += C._34;
//	_41 += C._41; _42 += C._42; _43 += C._43; _44 += C._44;
//
//	return *this;
//}
//
//complex_matrix4x4& complex_matrix4x4::operator-=(const complex_matrix4x4& C) {
//
//	_11 -= C._11; _12 -= C._12; _13 -= C._13; _14 -= C._14;
//	_21 -= C._21; _22 -= C._22; _23 -= C._23; _24 -= C._24;
//	_31 -= C._31; _32 -= C._32; _33 -= C._33; _34 -= C._34;
//	_41 -= C._41; _42 -= C._42; _43 -= C._43; _44 -= C._44;
//
//	return *this;
//}
//
//complex_matrix4x4& complex_matrix4x4::operator*=(const complex_matrix4x4& C){
//
//	complex_matrix4x4 CO;
//
//	CO._11 = _11 * C._11 + _12 * C._21 + _13 * C._31 + _14 * C._41;
//	CO._12 = _11 * C._12 + _12 * C._22 + _13 * C._32 + _14 * C._42;
//	CO._13 = _11 * C._13 + _12 * C._23 + _13 * C._33 + _14 * C._43;
//	CO._14 = _11 * C._14 + _12 * C._24 + _13 * C._34 + _14 * C._44;
//
//	CO._21 = _21 * C._11 + _22 * C._21 + _23 * C._31 + _24 * C._41;
//	CO._22 = _21 * C._12 + _22 * C._22 + _23 * C._32 + _24 * C._42;
//	CO._23 = _21 * C._13 + _22 * C._23 + _23 * C._33 + _24 * C._43;
//	CO._24 = _21 * C._14 + _22 * C._24 + _23 * C._34 + _24 * C._44;
//
//	CO._31 = _31 * C._11 + _32 * C._21 + _33 * C._31 + _34 * C._41;
//	CO._32 = _31 * C._12 + _32 * C._22 + _33 * C._32 + _34 * C._42;
//	CO._33 = _31 * C._13 + _32 * C._23 + _33 * C._33 + _34 * C._43;
//	CO._34 = _31 * C._14 + _32 * C._24 + _33 * C._34 + _34 * C._44;
//
//	CO._41 = _41 * C._11 + _42 * C._21 + _43 * C._31 + _44 * C._41;
//	CO._42 = _41 * C._12 + _42 * C._22 + _43 * C._32 + _44 * C._42;
//	CO._43 = _41 * C._13 + _42 * C._23 + _43 * C._33 + _44 * C._43;
//	CO._44 = _41 * C._14 + _42 * C._24 + _43 * C._34 + _44 * C._44;
//
//	 *this = CO;
//
//	 return *this;
//}
//
//complex_matrix4x4& complex_matrix4x4::operator*=(const float F){
//	complex_matrix4x4 C;
//	C._11 = _11 * F;
//	C._12 = _12 * F;
//	C._13 = _13 * F;
//	C._14 = _14 * F;
//
//	C._21 = _21 * F;
//	C._22 = _22 * F;
//	C._23 = _23 * F;
//	C._24 = _24 * F;
//
//	C._31 = _31 * F;
//	C._32 = _32 * F;
//	C._33 = _33 * F;
//	C._34 = _34 * F;
//
//	C._41 = _41 * F;
//	C._42 = _42 * F;
//	C._43 = _43 * F;
//	C._44 = _44 * F;
//
//	*this = C;
//	return *this;
//}
//
//complex_matrix4x4 complex_matrix4x4::operator+(const complex_matrix4x4& C) const {
//
//	complex_matrix4x4 CO;
//
//	CO._11 += C._11; CO._12 += C._12; CO._13 += C._13; CO._14 += C._14;
//	CO._21 += C._21; CO._22 += C._22; CO._23 += C._23; CO._24 += C._24;
//	CO._31 += C._31; CO._32 += C._32; CO._33 += C._33; CO._34 += C._34;
//	CO._41 += C._41; CO._42 += C._42; CO._43 += C._43; CO._44 += C._44;
//
//	return CO;
//}
//
//complex_matrix4x4 complex_matrix4x4::operator-(const complex_matrix4x4& C) const {
//
//	complex_matrix4x4 CO;
//
//	CO._11 -= C._11; CO._12 -= C._12; CO._13 -= C._13; CO._14 -= C._14;
//	CO._21 -= C._21; CO._22 -= C._22; CO._23 -= C._23; CO._24 -= C._24;
//	CO._31 -= C._31; CO._32 -= C._32; CO._33 -= C._33; CO._34 -= C._34;
//	CO._41 -= C._41; CO._42 -= C._42; CO._43 -= C._43; CO._44 -= C._44;
//
//	return CO;
//}
//
//complex_matrix4x4 complex_matrix4x4::operator*(const complex_matrix4x4& C) const {
//
//	complex_matrix4x4 CO;
//
//	CO._11 = _11 * C._11 + _12 * C._21 + _13 * C._31 + _14 * C._41;
//	CO._12 = _11 * C._12 + _12 * C._22 + _13 * C._32 + _14 * C._42;
//	CO._13 = _11 * C._13 + _12 * C._23 + _13 * C._33 + _14 * C._43;
//	CO._14 = _11 * C._14 + _12 * C._24 + _13 * C._34 + _14 * C._44;
//
//	CO._21 = _21 * C._11 + _22 * C._21 + _23 * C._31 + _24 * C._41;
//	CO._22 = _21 * C._12 + _22 * C._22 + _23 * C._32 + _24 * C._42;
//	CO._23 = _21 * C._13 + _22 * C._23 + _23 * C._33 + _24 * C._43;
//	CO._24 = _21 * C._14 + _22 * C._24 + _23 * C._34 + _24 * C._44;
//
//	CO._31 = _31 * C._11 + _32 * C._21 + _33 * C._31 + _34 * C._41;
//	CO._32 = _31 * C._12 + _32 * C._22 + _33 * C._32 + _34 * C._42;
//	CO._33 = _31 * C._13 + _32 * C._23 + _33 * C._33 + _34 * C._43;
//	CO._34 = _31 * C._14 + _32 * C._24 + _33 * C._34 + _34 * C._44;
//
//	CO._41 = _41 * C._11 + _42 * C._21 + _43 * C._31 + _44 * C._41;
//	CO._42 = _41 * C._12 + _42 * C._22 + _43 * C._32 + _44 * C._42;
//	CO._43 = _41 * C._13 + _42 * C._23 + _43 * C._33 + _44 * C._43;
//	CO._44 = _41 * C._14 + _42 * C._24 + _43 * C._34 + _44 * C._44;
//
//	return CO;
//}
//
//complex_matrix4x4 complex_matrix4x4::operator*(const float F) const{
//	complex_matrix4x4 C;
//	C._11 = _11 * F;
//	C._12 = _12 * F;
//	C._13 = _13 * F;
//	C._14 = _14 * F;
//
//	C._21 = _21 * F;
//	C._22 = _22 * F;
//	C._23 = _23 * F;
//	C._24 = _24 * F;
//
//	C._31 = _31 * F;
//	C._32 = _32 * F;
//	C._33 = _33 * F;
//	C._34 = _34 * F;
//
//	C._41 = _41 * F;
//	C._42 = _42 * F;
//	C._43 = _43 * F;
//	C._44 = _44 * F;
//
//	return C;
//}
//
//
//
//complex_matrix4x4 complex_matrix4x4::operator+() const {
//	return complex_matrix4x4(
//		_11, _12, _13, _14,
//		_21, _22, _23, _24,
//		_31, _32, _33, _34,
//		_41, _42, _43, _44);
//}
//
//complex_matrix4x4 complex_matrix4x4::operator-() const {
//	return complex_matrix4x4(
//		-_11, -_12, -_13, -_14,
//		-_21, -_22, -_23, -_24,
//		-_31, -_32, -_33, -_34,
//		-_41, -_42, -_43, -_44);
//}
//
//
//complex_matrix4x4 complex_matrix4x4::trans() {
//
//	complex_matrix4x4 C;
//	C = *this;
//
//	std::swap(C._12, C._21);
//	std::swap(C._13, C._31);
//	std::swap(C._14, C._41);
//	std::swap(C._23, C._32);
//	std::swap(C._24, C._42);
//	std::swap(C._34, C._43);
//
//	return C;
//}
//
//complex_matrix4x4 complex_matrix4x4::conj() {
//
//	complex_matrix4x4 C = (*this).trans();
//
//	C._11 = complex::conj(C._11);
//	C._12 = complex::conj(C._12);
//	C._13 = complex::conj(C._13);
//	C._14 = complex::conj(C._14);
//
//	C._21 = complex::conj(C._21);
//	C._22 = complex::conj(C._22);
//	C._23 = complex::conj(C._23);
//	C._24 = complex::conj(C._24);
//
//	C._31 = complex::conj(C._31);
//	C._32 = complex::conj(C._32);
//	C._33 = complex::conj(C._33);
//	C._34 = complex::conj(C._34);
//
//	C._41 = complex::conj(C._41);
//	C._42 = complex::conj(C._42);
//	C._43 = complex::conj(C._43);
//	C._44 = complex::conj(C._44);
//
//	return C;
//}
//
//complex complex_matrix4x4::det2x2() {
//	return _11 * _22 - _12 * _21;
//}
//
//complex complex_matrix4x4::det3x3() {
//	return 
//		_11 * _22 * _33 + 
//		_12 * _23 * _31 + 
//		_13 * _21 * _32 - 
//		_11 * _23 * _32 - 
//		_12 * _21 * _33 - 
//		_13 * _22 * _31;
//}
//
//
//#pragma endregion