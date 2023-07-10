#pragma once

#include"Actor.h"
#include"Gmath.h"
#include<vector>

struct SphpereColision
{
	Actor* owner;
	float radius;

	SphpereColision() :owner(nullptr), radius(1.0f) {};
	SphpereColision(Actor* actor, float r) :owner(actor), radius(r) {};
};

bool intersect(SphpereColision s1, SphpereColision s2);

struct AABBCollision
{
	Actor* owner;
	XMFLOAT3 curMax;
	XMFLOAT3 curMin;
	XMFLOAT3 oriMax;
	XMFLOAT3 oriMin;

	AABBCollision();
	AABBCollision(Actor* actor, std::vector<XMFLOAT3>& points);
	AABBCollision(Actor* actor, XMFLOAT3 size, XMFLOAT3 offset);
	void rotate();
};

bool intersect(AABBCollision s1, AABBCollision s2);

struct PlaneCollision
{
	XMFLOAT3 normal;
	float d;

	PlaneCollision();
	PlaneCollision(XMFLOAT3 nVec, XMFLOAT3 pos);
};

bool intersect(SphpereColision s, PlaneCollision p);

XMFLOAT3 calcFixVector(AABBCollision b1, AABBCollision b2);