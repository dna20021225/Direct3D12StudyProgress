#include "Collision.h"
#include<cmath>

bool intersect(SphpereColision s1, SphpereColision s2)
{
	float r2 = (s1.radius + s2.radius) * (s1.radius + s2.radius);
	return(r2 > lengthSq(s2.owner->getPos() - s1.owner->getPos())) ? true : false;
}

AABBCollision::AABBCollision()
	: owner(nullptr)
	, curMax(ZeroVec3d)
	, curMin(ZeroVec3d)
	, oriMax(ZeroVec3d)
	, oriMin(ZeroVec3d)
{

}

AABBCollision::AABBCollision(Actor* actor, std::vector<XMFLOAT3>& points)
	: owner(actor)
	, curMax(ZeroVec3d)
	, curMin(ZeroVec3d)
	, oriMax(ZeroVec3d)
	, oriMin(ZeroVec3d)
{
	oriMax = points[0];
	oriMin = points[0];

	for (size_t i = 1; i < points.size(); ++i)
	{
		if (oriMax.x < points[i].x)oriMax.x = points[i].x;
		if (oriMin.x > points[i].x)oriMin.x = points[i].x;
		if (oriMax.y < points[i].y)oriMax.y = points[i].y;
		if (oriMin.y > points[i].y)oriMin.y = points[i].y;
		if (oriMax.z < points[i].z)oriMax.z = points[i].z;
		if (oriMin.z > points[i].z)oriMin.z = points[i].z;
	}
	curMax = oriMax;
	curMin = oriMin;
}

AABBCollision::AABBCollision(Actor* actor, XMFLOAT3 size, XMFLOAT3 offset)
	: owner(actor)
	, oriMax(offset + 0.5f * size)
	, oriMin(offset - 0.5f * size)
{
	curMax = oriMax;
	curMin = oriMin;
}

void AABBCollision::rotate()
{
	XMMATRIX mat = owner->getWorldMatrix();

	std::vector<XMFLOAT3>points;
	points.push_back(XMFLOAT3(oriMax.x, oriMax.y, oriMax.z));
	points.push_back(XMFLOAT3(oriMin.x, oriMax.y, oriMax.z));
	points.push_back(XMFLOAT3(oriMax.x, oriMin.y, oriMax.z));
	points.push_back(XMFLOAT3(oriMax.x, oriMax.y, oriMin.z));
	points.push_back(XMFLOAT3(oriMin.x, oriMin.y, oriMax.z));
	points.push_back(XMFLOAT3(oriMin.x, oriMax.y, oriMin.z));
	points.push_back(XMFLOAT3(oriMax.x, oriMin.y, oriMin.z));
	points.push_back(XMFLOAT3(oriMin.x, oriMin.y, oriMin.z));

	XMFLOAT3 pos = multipleRot(points[0], mat);
	curMax = pos;
	curMin = pos;
	for (size_t i = 1; i < points.size(); ++i)
	{
		XMFLOAT3 pos = multipleRot(points[i], mat);
		if (curMax.x < points[i].x)curMax.x = pos.x;
		if (curMin.x > points[i].x)curMin.x = pos.x;
		if (curMax.y < points[i].y)curMax.y = pos.y;
		if (curMin.y > points[i].y)curMin.y = pos.y;
		if (curMax.z < points[i].z)curMax.z = pos.z;
		if (curMin.z > points[i].z)curMin.z = pos.z;
	}
}

bool intersect(AABBCollision b1, AABBCollision b2)
{
	XMFLOAT3 pos1 = b1.owner->getPos();
	XMFLOAT3 pos2 = b2.owner->getPos();

	if (b1.curMax.x + pos1.x < b2.curMin.x + pos2.x || b2.curMax.x + pos2.x < b1.curMin.x + pos1.x)return false;
	if (b1.curMax.y + pos1.y < b2.curMin.y + pos2.y || b2.curMax.y + pos2.y < b1.curMin.y + pos1.y)return false;
	if (b1.curMax.z + pos1.z < b2.curMin.z + pos2.z || b2.curMax.z + pos2.z < b1.curMin.z + pos1.z)return false;

	return true;
}

PlaneCollision::PlaneCollision()
	: normal(UnitVecZ3d)
	, d(0.0f)
{

}

PlaneCollision::PlaneCollision(XMFLOAT3 nVec, XMFLOAT3 pos)
	:normal(normalize(nVec))
{
	d = -dot(normal, pos);
}

bool intersect(SphpereColision s, PlaneCollision p)
{
	float dist = dot(s.owner->getPos(), p.normal) - p.d;

	return (std::fabs(dist) < s.radius) ? true : false;
}

XMFLOAT3 calcFixVector(AABBCollision b1, AABBCollision b2)
{
	XMFLOAT3 pos1 = b1.owner->getPos();
	XMFLOAT3 pos2 = b2.owner->getPos();

	XMFLOAT3 diff1((b2.curMax.x + pos2.x) - (b1.curMin.x + pos1.x),
		(b2.curMax.y + pos2.y) - (b1.curMin.y + pos1.y),
		(b2.curMax.z + pos2.z) - (b1.curMin.z + pos1.z));

	XMFLOAT3 diff2((b2.curMin.x + pos2.x) - (b1.curMax.x + pos1.x),
		(b2.curMin.y + pos2.y) - (b1.curMax.y + pos1.y),
		(b2.curMin.z + pos2.z) - (b1.curMax.z + pos1.z));

	XMFLOAT3 diff;
	diff.x = (std::fabs(diff1.x) < std::fabs(diff2.x)) ? diff1.x : diff2.x;
	diff.y = (std::fabs(diff1.y) < std::fabs(diff2.y)) ? diff1.y : diff2.y;
	diff.z = (std::fabs(diff1.z) < std::fabs(diff2.z)) ? diff1.z : diff2.z;

	XMFLOAT3 adiff(std::fabs(diff.x), std::fabs(diff.y), std::fabs(diff.z));
	if (adiff.x <= adiff.y)
	{
		if (adiff.x <= adiff.z)
			return XMFLOAT3(diff.x, 0.0f, 0.0f);
		else
			return XMFLOAT3(0.0f, 0.0f, diff.z);
	}
	else if (adiff.y < adiff.z)
	{
		return XMFLOAT3(0.0f, diff.y, 0.0f);
	}
	return XMFLOAT3(0.0f, 0.0f, diff.z);
}