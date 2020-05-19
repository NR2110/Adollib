#include "vector.h"

#include <math.h>
#include"quaternion.h"
using namespace Adollib;

//::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region vector3
//::::::::::::::::::::::::::::::::::::::::::::::::

//operator
vector3 vector3::operator+ () const {
	return *this;
}
vector3 vector3::operator- () const {
	vector3 R = *this;
	R.x *= -1;
	R.y *= -1;
	R.z *= -1;
	return R;
}

vector3 vector3::operator+ (const vector3& M) const {
	vector3 R;
	R.x = x + M.x;
	R.y = y + M.y;
	R.z = z + M.z;
	return R;
}
vector3 vector3::operator- (const vector3& M) const {
	vector3 R;
	R.x = x - M.x;
	R.y = y - M.y;
	R.z = z - M.z;
	return R;
}
vector3 vector3::operator* (const vector3& M) const{
	vector3 R;
	R.x = x * M.x;
	R.y = y * M.y;
	R.z = z * M.z;
	return R;
}
vector3 vector3::operator* (float S) const {
	vector3 R;
	R.x = x * S;
	R.y = y * S;
	R.z = z * S;
	return R;
}
vector3 Adollib::operator*(float S, const vector3& M) {
	vector3 R;
	R.x = M.x * S;
	R.y = M.y * S;
	R.z = M.z * S;
	return R;
}

vector3 vector3::operator/ (float S) const {
	vector3 R;
	R.x = x / S;
	R.y = y / S;
	R.z = z / S;
	return R;
}
float& vector3::operator[] (const int i) {
	switch (i)
	{
	case 0: return x;
		break;
	case 1: return y;
		break;
	case 2: return z;
		break;
	}
	float R;
	return R;
}
float vector3::operator[] (const int i) const{
	switch (i)
	{
	case 0: return x;
		break;
	case 1: return y;
		break;
	case 2: return z;
		break;
	}
	return 0;
}


vector3& vector3::operator+= (const vector3& M) {
	x = x + M.x;
	y = y + M.y;
	z = z + M.z;
	return *this;
}
vector3& vector3::operator-= (const vector3& M) {
	x = x - M.x;
	y = y - M.y;
	z = z - M.z;
	return *this;
}
vector3& vector3::operator*= (float S) {
	x = x * S;
	y = y * S;
	z = z * S;
	return *this;
}
vector3& vector3::operator/= (float S) {
	x = x / S;
	y = y / S;
	z = z / S;
	return *this;
}

bool vector3::operator== (const vector3& M) {
	if (x == M.x && y == M.y && z == M.z)return true;
	return false;
}
bool vector3::operator!= (const vector3& M) {
	if (x == M.x && y == M.y && z == M.z)return false;
	return true;
}

DirectX::XMFLOAT3 vector3::get_XM3() {
	DirectX::XMFLOAT3 R;
	R.x = x;
	R.y = y;
	R.z = z;
	return R;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
float vector3::norm_sqr() {
	float S = norm();
	if (normed * normed == S) return normed;
	else if (S == 1.0f) normed = 1.0f;
	else normed = sqrtf(S);
	return normed;
}

float vector3::norm_sqr() const{
	float S = norm();
	if (normed * normed == S) return normed;
	if (S == 1.0f) return 1.0f;
	else return sqrtf(S);
}

float vector3::norm() const {
	return x * x + y * y + z * z;
}

vector3 vector3::unit_vect() {
	float V = norm_sqr();
	if (V == 1.0f)return *this;
	if (V < FLT_EPSILON)return vector3(0, 0, 0);
	vector3 R = *this;
	return R / V;
}
vector3 vector3::unit_vect() const {
	float V = norm_sqr();
	vector3 R = *this;
	return R / V;
}

float Adollib::vector3_distance_sqr(const vector3& P, const vector3& Q) {
	return 
		sqrtf((P.x - Q.x) * (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y) + (P.z - Q.z) * (P.z - Q.z));
}

float Adollib::vector3_distance(const vector3& P, const vector3& Q) {
	return (P.x - Q.x)* (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y) + (P.z - Q.z) * (P.z - Q.z);
}


float Adollib::vector3_dot(const vector3& P, const vector3& Q) {
	return P.x * Q.x + P.y * Q.y + P.z * Q.z;
}


vector3 Adollib::vector3_cross(const vector3& P, const vector3& Q) {
	vector3 R;
	R.x = P.y * Q.z - P.z * Q.y;
	R.y = P.z * Q.x - P.x * Q.z;
	R.z = P.x * Q.y - P.y * Q.x;

	if (isnan(R.x)) {
		float A = P.y * Q.z;
		float B = P.z * Q.y;
		if (isinf(A)) (A = A > 0 ? FLT_MAX : -FLT_MAX);
		if (isinf(B)) (B = B > 0 ? FLT_MAX : -FLT_MAX);

		R.x = A - B;
	}
	if (isnan(R.y)) {
		float A = P.z * Q.x ;
		float B = P.x * Q.z;
		if (isinf(A)) (A = A > 0 ? FLT_MAX : -FLT_MAX);
		if (isinf(B)) (B = B > 0 ? FLT_MAX : -FLT_MAX);

		R.y = A - B;
	}
	if (isnan(R.z)) {
		float A = P.x * Q.y;
		float B = P.y * Q.x;
		if (isinf(A)) (A = A > 0 ? FLT_MAX : -FLT_MAX);
		if (isinf(B)) (B = B > 0 ? FLT_MAX : -FLT_MAX);

		R.z = A - B;
	}
	if (isinf(R.x)) R.x = R.x > 0 ? R.x = FLT_MAX : -FLT_MAX;
	if (isinf(R.y)) R.y = R.y > 0 ? R.y = FLT_MAX : -FLT_MAX;
	if (isinf(R.z)) R.z = R.z > 0 ? R.z = FLT_MAX : -FLT_MAX;

	return R;
}


float Adollib::vector3_angle(const vector3& P, const vector3& Q) {

	return ToAngle(acosf(Adollib::vector3_dot(P.unit_vect(), Q.unit_vect())));
}
float Adollib::vector3_radian(const vector3& P, const vector3& Q) {

	return acosf(Adollib::vector3_dot(P.unit_vect(), Q.unit_vect()));
}

vector3 Adollib::vector3_look_at(const vector3& P, const vector3& Q) {
	return Q - P;
}

vector3 Adollib::vector3_slerp(const vector3& V1, const vector3& V2, float R) {
	vector3 g = vector3_cross(V1, V2);
	float radian = vector3_radian(V1, V2) / 2.0f;

	quaternion F;
	F.w = cosf(radian * R);
	F.x = g.x * sinf(radian * R);
	F.y = g.y * sinf(radian * R);
	F.z = g.z * sinf(radian * R);

	return vector3_be_rotated_by_quaternion(V1, F);
}


#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::

//::::::::::::::::::::::::::::::::::::::::::::::::
#pragma region vector2
//::::::::::::::::::::::::::::::::::::::::::::::::
//operator
n_vector2 n_vector2::operator+ () const {
	return *this;
}
n_vector2 n_vector2::operator- () const {
	n_vector2 R = *this;
	R.x *= -1;
	R.y *= -1;
	return R;
}

n_vector2 n_vector2::operator+ (const n_vector2& M) const {
	n_vector2 R;
	R.x = x + M.x;
	R.y = y + M.y;
	return R;
}
n_vector2 n_vector2::operator- (const n_vector2& M) const {
	n_vector2 R;
	R.x = x - M.x;
	R.y = y - M.y;
	return R;
}
n_vector2 n_vector2::operator* (const n_vector2& M) const {
	n_vector2 R;
	R.x = x * M.x;
	R.y = y * M.y;
	return R;
}
n_vector2 n_vector2::operator* (float S) const {
	n_vector2 R;
	R.x = x * S;
	R.y = y * S;
	return R;
}
n_vector2 Adollib::operator*(float S, const n_vector2& M) {
	n_vector2 R;
	R.x = M.x * S;
	R.y = M.y * S;
	return R;
}

n_vector2 n_vector2::operator/ (float S) const {
	n_vector2 R;
	R.x = x / S;
	R.y = y / S;
	return R;
}

n_vector2& n_vector2::operator+= (const n_vector2& M) {
	x = x + M.x;
	y = y + M.y;
	return *this;
}
n_vector2& n_vector2::operator-= (const n_vector2& M) {
	x = x - M.x;
	y = y - M.y;
	return *this;
}
n_vector2& n_vector2::operator*= (float S) {
	x = x * S;
	y = y * S;
	return *this;
}
n_vector2& n_vector2::operator/= (float S) {
	x = x / S;
	y = y / S;
	return *this;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
float n_vector2::norm_sqr() {
	float S = norm();
	if (normed * normed == S) return normed;
	else if (S == 1.0f) normed = 1.0f;
	else normed = sqrtf(S);
}
float n_vector2::norm_sqr() const {
	float S = norm();
	if (normed * normed == S) return normed;
	if (S == 1.0f) return 1.0f;
	else return sqrtf(S);
}

float n_vector2::norm() const {
	return x * x + y * y;
}

n_vector2 n_vector2::unit_vect() {
	float V = norm_sqr();
	n_vector2 R = *this;
	return R / V;
}
n_vector2 n_vector2::unit_vect() const {
	float V = norm_sqr();
	n_vector2 R = *this;
	return R / V;
}
float Adollib::vector2_distance_sqr(const n_vector2& P, const n_vector2& Q) {
	return
		sqrtf((P.x - Q.x) * (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y));
}

float Adollib::vector2_distance(const n_vector2& P, const n_vector2& Q) {
	return (P.x - Q.x) * (P.x - Q.x) + (P.y - Q.y) * (P.y - Q.y);
}


float Adollib::vector2_dot(const n_vector2& P, const n_vector2& Q) {
	return P.x * Q.x + P.y * Q.y;
}

float Adollib::vector2_angle(const n_vector2& P, const n_vector2& Q) {

	return ToAngle(acosf(Adollib::vector2_dot(P.unit_vect(), Q.unit_vect())));
}

n_vector2 Adollib::vector2_look_at(const n_vector2& P, const n_vector2& Q) {
	return Q - P;
}


#pragma endregion
//::::::::::::::::::::::::::::::::::::::::::::::::

float Adollib::ToRadian(float angle) {
	return (float)angle * (3.141592654f / 180.0f);
}

float Adollib::ToAngle(float radian) {
	return (float)radian * 180 / 3.1415926535f;
}