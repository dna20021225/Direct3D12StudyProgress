#pragma once

#include <DirectXMath.h>
using namespace DirectX;

// 2D

// ���Z�q�I�[�o�[���[�h
XMFLOAT2 operator +(const XMFLOAT2& a, const XMFLOAT2& b);
XMFLOAT2 operator -(const XMFLOAT2& a, const XMFLOAT2& b);
XMFLOAT2 operator *(const XMFLOAT2& a, float c);
XMFLOAT2 operator *(float c, const XMFLOAT2& a);

void operator += (XMFLOAT2& a, const XMFLOAT2& b);
void operator -=(XMFLOAT2& a, const XMFLOAT2& b);
void operator *= (XMFLOAT2& a, float c);

// �x�N�g���֌W
float length(const XMFLOAT2& a);
float lengthSq(const XMFLOAT2& a);
XMFLOAT2 normalize(const XMFLOAT2& a);

float dot(const XMFLOAT3& a, const XMFLOAT3& b);
float cross(const XMFLOAT2& a, const XMFLOAT2& b);

XMFLOAT2 unitVecAngle(float theta);

void operator += (XMFLOAT3& a, const XMFLOAT3& b);
void operator -=(XMFLOAT3& a, const XMFLOAT3& b);
void operator *= (XMFLOAT3& a, float c);

//�萔
const XMFLOAT2 UnitVecX(1.0f, 0.0f);
const XMFLOAT2 UnitVecY(0.0f, 1.0f);
const XMFLOAT2 ZeroVec(0.0f, 0.0f);

// 3D

// ���Z�q�I�[�o�[���[�h
XMFLOAT3 operator +(const XMFLOAT3& a, const XMFLOAT3& b);
XMFLOAT3 operator -(const XMFLOAT3& a, const XMFLOAT3& b);
XMFLOAT3 operator *(const XMFLOAT3& a, float c);
XMFLOAT3 operator *(float c, const XMFLOAT3& a);

void operator += (XMFLOAT3& a, const XMFLOAT3& b);
void operator -=(XMFLOAT3& a, const XMFLOAT3& b);
void operator *= (XMFLOAT3& a, float c);

// �x�N�g���֌W
float length(const XMFLOAT3& a);
float lengthSq(const XMFLOAT3& a);
XMFLOAT3 normalize(const XMFLOAT3& a);

float dot(const XMFLOAT3& a, const XMFLOAT3& b);
XMFLOAT3 cross(const XMFLOAT3& a, const XMFLOAT3& b);

//�萔
const XMFLOAT3 UnitVecX3d(1.0f, 0.0f, 0.0f);
const XMFLOAT3 UnitVecY3d(0.0f, 1.0f, 0.0f);
const XMFLOAT3 UnitVecZ3d(0.0f, 0.0f, 1.0f);
const XMFLOAT3 ZeroVec3d(0.0f, 0.0f, 0.0f);


// ����
struct Segment
{
	XMFLOAT2 start;
	XMFLOAT2 end;

	Segment() : start(ZeroVec), end(UnitVecX) {}
	Segment(const XMFLOAT2& a, const XMFLOAT2& b) : start(a), end(b) {}
};

// �~
struct Circle
{
	XMFLOAT2 pos;
	float r;

	Circle() : pos(ZeroVec), r(1.0f) {}
	Circle(const XMFLOAT2& p, float rad) : pos(p), r(rad) {}
};

// ��` (float�^)
//struct Rect
//{
//	float left, top, right, bottom;
//
//	Rect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
//	Rect(float lt, float tp, float rt, float bm) : left(lt), top(tp), right(rt), bottom(bm) {}
//};

// ��`����4�̐����ɕ���
//void separateRect(const Rect& rect, Segment& sl, Segment& sr, Segment& st, Segment& sb);

XMFLOAT3 multipleRot(XMFLOAT3 pos, XMMATRIX mat);