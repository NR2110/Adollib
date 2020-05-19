//#pragma once
//
//#include "../vector.h"
//
//namespace Adollib {
//
//	// ï°ëfêîópclass
//	class complex {
//	private:
//		float x;
//		float y;
//
//	public:
//		complex() { x = 0; y = 0; }
//		complex(VECTOR2 V) { x = V.x; y = V.y; }
//		complex(float xx, float yy) { x = xx; y = yy; }
//		complex(float xx) { x = xx; y = 0; }
//		~complex() {}
//
//		complex operator+() const;
//		complex operator-() const;
//
//		complex operator+ (const complex&) const;
//		complex operator- (const complex&) const;
//		complex operator* (const complex&) const;
//		complex operator* (const float) const;
//		//complex operator/ (const complex&) const;
//
//		complex& operator= (const complex&);
//		complex& operator+= (const complex&);
//		complex& operator-= (const complex&);
//		complex& operator*= (const complex&);
//		complex& operator*= (const float);
//
//		bool operator== (const complex&) const;
//		bool operator!= (const complex&) const;
//
//		// static //
//		static complex ident() { return complex(1, 0); }
//		static complex zero() { return complex(0, 0); }
//		static complex conj(const complex& C) { return complex(C.x, -C.y); }
//	};
//
//	// ï°ëfêîópÇÃçsóÒclass
//	class complex_matrix4x4 {
//	private:
//		complex _11;
//		complex _12;
//		complex _13;
//		complex _14;
//		complex _21;
//		complex _22;
//		complex _23;
//		complex _24;
//		complex _31;
//		complex _32;
//		complex _33;
//		complex _34;
//		complex _41;
//		complex _42;
//		complex _43;
//		complex _44;
//
//
//	public:
//
//		complex_matrix4x4() {
//			_11 = _12 = _13 = _14 =
//			_21 = _22 = _23 = _24 = 
//			_31 = _32 = _33 = _34 = 
//			_41 = _42 = _43 = _44 = 
//			complex::zero();
//		}
//
//		////ëŒäpÇà¯êîÇ≈èâä˙âªÅ@ëºÇÕ0
//		//complex_matrix4x4(float V) {
//		//	_11 = _12 = _13 = _14 =
//		//	_21 = _22 = _23 = _24 =
//		//	_31 = _32 = _33 = _34 =
//		//	_41 = _42 = _43 = _44 =
//		//	complex::zero();
//
//		//	_11 = _22 = _33 = _44 = complex(V);		
//		//}
//
//		////ëŒäpÇà¯êîÇ≈èâä˙âªÅ@ëºÇÕ0
//		//complex_matrix4x4(complex V) {
//		//	_11 = _12 = _13 = _14 =
//		//		_21 = _22 = _23 = _24 =
//		//		_31 = _32 = _33 = _34 =
//		//		_41 = _42 = _43 = _44 =
//		//		complex::zero();
//
//		//	_11 = _22 = _33 = _44 = V;
//		//}
//
//		complex_matrix4x4(
//			complex c11, complex c12, complex c13, complex c14,
//			complex c21, complex c22, complex c23, complex c24,
//			complex c31, complex c32, complex c33, complex c34,
//			complex c41, complex c42, complex c43, complex c44
//		) { 
//			_11 = c11; _12 = c12; _13 = c13; _14 = c14;
//			_21 = c11; _22 = c12; _23 = c13; _24 = c24;
//			_31 = c11; _32 = c12; _33 = c13; _34 = c34;
//			_41 = c11; _42 = c12; _43 = c13; _44 = c44;
//		}
//
//		complex_matrix4x4(const complex_matrix4x4& C) {
//			_11 = C._11; _12 = C._12; _13 = C._13; _14 = C._14;
//			_21 = C._21; _22 = C._22; _23 = C._23; _24 = C._24;
//			_31 = C._31; _32 = C._32; _33 = C._33; _34 = C._34;
//			_41 = C._41; _42 = C._42; _43 = C._43; _44 = C._44;
//		}
//
//		complex_matrix4x4& operator=(const complex_matrix4x4&);
//		complex_matrix4x4& operator+=(const complex_matrix4x4&);
//		complex_matrix4x4& operator-=(const complex_matrix4x4&);
//		complex_matrix4x4& operator*=(const complex_matrix4x4&);
//		complex_matrix4x4& operator*=(const float);
//
//		complex_matrix4x4 operator+() const;
//		complex_matrix4x4 operator-() const;
//
//		complex_matrix4x4 operator+(const complex_matrix4x4&) const;
//		complex_matrix4x4 operator-(const complex_matrix4x4&) const;
//		complex_matrix4x4 operator*(const complex_matrix4x4&) const;
//		complex_matrix4x4 operator*(const float) const;
//
//		// ä÷êî //
//
//		complex_matrix4x4 trans();
//
//		complex_matrix4x4 conj();
//
//		complex tr() { return _11 + _22 + _33 + _44; }
//
//		//ÇªÇÒÇ»Ç‡ÇÃÇÕÇ»Ç¢!! Ç»Ç∫Ç»ÇÁÇﬂÇÒÇ«Ç≠Ç≥Ç¢Ç©ÇÁÇæ!!!
//		complex_matrix4x4 invers();
//
//		complex det2x2();
//
//		complex det3x3();
//
//		// static //
//		static complex_matrix4x4 ident() {
//			return complex_matrix4x4(
//				complex(1,0),	  complex::zero(), complex::zero(), complex::zero(),
//				complex::zero(), complex(1,0),		complex::zero(), complex::zero(),
//				complex::zero(), complex::zero(), complex(1,0),	  complex::zero(),
//				complex::zero(), complex::zero(), complex::zero(), complex(1,0)
//			);
//
//		}
//
//		static complex_matrix4x4 zero() {
//			return complex_matrix4x4(
//				0,0,0,0,
//				0,0,0,0,
//				0,0,0,0,
//				0,0,0,0
//			);
//
//		}
//
//
//	};
//
//}