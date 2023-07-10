#pragma once

#include "GMath.h"


struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

struct TransformMat
{
	XMMATRIX posMat;
	XMMATRIX uvMat;
};

struct VertexNorm
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	XMFLOAT3 norm;
};

struct ParalleLight
{
	XMFLOAT4 direction;
	XMFLOAT4 color;

	ParalleLight();
	ParalleLight(XMFLOAT3 dir, XMFLOAT3 col);
};

struct MeshMaterial
{
	XMFLOAT4 kDiffuse;
	XMFLOAT4 kSpecular;
	XMFLOAT4 kAmbient;

	MeshMaterial();
	MeshMaterial(XMFLOAT3 diffuse, XMFLOAT3 specular, float power, XMFLOAT3 ambient);
};

struct ConstData
{
	XMFLOAT4 eye;
	ParalleLight light;
	MeshMaterial material;
	XMMATRIX world;
	XMMATRIX viewProj;

	ConstData();
};

struct ColorRGBA
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	ColorRGBA() :r(0), g(0), b(0), a(255) {}
	ColorRGBA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
		:r(red), g(green), b(blue), a(alpha) {}
};


extern const wchar_t* WHITE_TEXTRE;
extern const wchar_t* BLACL_TEXTRE;
extern const wchar_t* RED_TEXTRE;
extern const wchar_t* GREEN_TEXTRE;
extern const wchar_t* BLUE_TEXTRE;
extern const wchar_t* YELLOW_TEXTRE;
extern const wchar_t* CYAN_TEXTRE;
extern const wchar_t* MAGENTA_TEXTRE;