#include "GMath.h"
#include <cmath>

// 2D
XMFLOAT2 operator +(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return XMFLOAT2(a.x + b.x, a.y + b.y);
}

XMFLOAT2 operator -(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return XMFLOAT2(a.x - b.x, a.y - b.y);
}

XMFLOAT2 operator *(const XMFLOAT2& a, float c)
{
	return XMFLOAT2(a.x * c, a.y * c);
}

XMFLOAT2 operator *(float c, const XMFLOAT2& a)
{
	return XMFLOAT2(a.x * c, a.y * c);
}

void operator += (XMFLOAT2& a, const XMFLOAT2& b)
{
	a.x += b.x;
	a.y += b.y;
}

void operator -= (XMFLOAT2& a, const XMFLOAT2& b)
{
	a.x -= b.x;
	a.y -= b.y;
}

void operator *= (XMFLOAT2& a, float c)
{
	a.x *= c;
	a.y *= c;
}

float length(const XMFLOAT2& a)
{
	float sq = lengthSq(a);
	sq = (sq >= 0.0f) ? sq : 0.0f;
	return std::sqrt(sq);
}

float lengthSq(const XMFLOAT2& a)
{
	return (a.x * a.x) + (a.y * a.y);
}

XMFLOAT2 normalize(const XMFLOAT2& a)
{
	float len = length(a);
	if (std::fabs(len) < 0.01f) return ZeroVec;

	XMFLOAT2 b = a;
	float c = 1.0f / len;
	b.x /= len;
	b.y /= len;

	return b;
}

float dot(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return a.x * b.x + a.y * b.y;
}

float cross(const XMFLOAT2& a, const XMFLOAT2& b)
{
	return a.x * b.y - a.y * b.x;
}

XMFLOAT2 unitVecAngle(float theta)
{
	return XMFLOAT2(std::cos(theta), std::sin(theta));
}

// 3D
XMFLOAT3 operator +(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}

XMFLOAT3 operator -(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
}

XMFLOAT3 operator *(const XMFLOAT3& a, float c)
{
	return XMFLOAT3(a.x * c, a.y * c, a.z * c);
}

XMFLOAT3 operator *(float c, const XMFLOAT3& a)
{
	return XMFLOAT3(a.x * c, a.y * c, a.z * c);
}

void operator += (XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

void operator -= (XMFLOAT3& a, const XMFLOAT3& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
}

void operator *= (XMFLOAT3& a, float c)
{
	a.x *= c;
	a.y *= c;
	a.z *= c;
}

float length(const XMFLOAT3& a)
{
	float sq = lengthSq(a);
	sq = (sq >= 0.0f) ? sq : 0.0f;
	return std::sqrt(sq);
}

float lengthSq(const XMFLOAT3& a)
{
	return (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
}

XMFLOAT3 normalize(const XMFLOAT3& a)
{
	float len = length(a);
	if (std::fabs(len) < 0.01f) return ZeroVec3d;

	XMFLOAT3 b = a;
	float c = 1.0f / len;
	b.x *= c;
	b.y *= c;
	b.z *= c;

	return b;
}

float dot(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

XMFLOAT3 cross(const XMFLOAT3& a, const XMFLOAT3& b)
{
	return XMFLOAT3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

/*void separateRect(const Rect& rect, Segment& sl, Segment& sr, Segment& st, Segment& sb)
{
	XMFLOAT2 lt(rect.left, rect.top);
	XMFLOAT2 lb(rect.left, rect.bottom);
	XMFLOAT2 rt(rect.right, rect.top);
	XMFLOAT2 rb(rect.right, rect.bottom);

	sl = Segment(lt, lb);	//¶•Ó
	sr = Segment(rt, rb);	//‰E•Ó
	st = Segment(lt, rt);	//ã•Ó
	sb = Segment(lb, rb);	//‰º•Ó
}*/

XMFLOAT3 multipleRot(XMFLOAT3 pos, XMMATRIX mat)
{
	XMFLOAT3 p;

	p.x = pos.x * mat.r[0].m128_f32[0] + pos.y * mat.r[1].m128_f32[0] + pos.z * mat.r[2].m128_f32[0];
	p.y = pos.x * mat.r[0].m128_f32[1] + pos.y * mat.r[1].m128_f32[1] + pos.z * mat.r[2].m128_f32[1];
	p.z = pos.x * mat.r[0].m128_f32[2] + pos.y * mat.r[1].m128_f32[2] + pos.z * mat.r[2].m128_f32[2];

	return p;
}