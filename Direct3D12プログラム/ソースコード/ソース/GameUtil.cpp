#include"GameUtil.h"

extern const wchar_t* WHITE_TEXTRE = L"White";
extern const wchar_t* BLACL_TEXTRE = L"Black";
extern const wchar_t* RED_TEXTRE = L"Red";
extern const wchar_t* GREEN_TEXTRE = L"Green";
extern const wchar_t* BLUE_TEXTRE = L"Blue";
extern const wchar_t* YELLOW_TEXTRE = L"Yellow";
extern const wchar_t* CYAN_TEXTRE = L"Cyan";
extern const wchar_t* MAGENTA_TEXTRE = L"Magenta";

ParalleLight::ParalleLight()
{
	direction = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

ParalleLight::ParalleLight(XMFLOAT3 dir, XMFLOAT3 col)
{
	XMFLOAT3 ndir = normalize(dir);
	direction = XMFLOAT4(ndir.x, ndir.y, ndir.z, 0.0f);
	color = XMFLOAT4(col.x, col.y, col.z, 1.0f);
}

MeshMaterial::MeshMaterial()
{
	kDiffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);

	kSpecular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	kAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);

}

MeshMaterial::MeshMaterial(XMFLOAT3 diffuse, XMFLOAT3 specular, float power,
	XMFLOAT3 ambient)
{
	kDiffuse = XMFLOAT4(diffuse.x, diffuse.y, diffuse.z, 0.0f);

	kSpecular = XMFLOAT4(specular.x, specular.y, specular.z, power);

	kAmbient = XMFLOAT4(ambient.x, ambient.y, ambient.z, 0.0f);
}

ConstData::ConstData()
{
	eye = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	world = XMMatrixIdentity();

	viewProj = XMMatrixIdentity();

}