#pragma once 

#include <DirectXMath.h> 
using namespace DirectX;

struct CamInParam
{
	float fovY;
	float znear;
	float zfar;

	CamInParam() :fovY(90.0f), znear(1.0f), zfar(10.0f) {}
	CamInParam(float ifov, float inear, float ifar)
		: fovY(ifov), znear(inear), zfar(ifar) {}
};

struct CamExtParam
{
	XMFLOAT3 eye;
	XMFLOAT3 target;
	XMFLOAT3 up;

	CamExtParam() : eye(0.0f, 0.0f, 0.0f), target(1.0f, 1.0f, 1.0f),
		up(0.0f, 1.0f, 0.0f) {}
	CamExtParam(XMFLOAT3 ieye, XMFLOAT3 itarget, XMFLOAT3 iup)
		: eye(ieye), target(itarget), up(iup) {}
};

class Camera
{
public:
	Camera();
	Camera(CamInParam inParam, CamExtParam extParam);
	~Camera();
	CamInParam getlnParam() const { return m_inParam; }
	void setlnParam(CamInParam inParam) { m_inParam = inParam; }
	CamExtParam getExtParam() const { return m_extParam; }
	void setExtParam(CamExtParam extParam) { m_extParam = extParam; }
	XMMATRIX calcViewMatrix() const;
	XMMATRIX calcProjMatrix(float w, float h) const;
	XMMATRIX calcViewProjMatrix(float w, float h) const;
private:
	CamInParam m_inParam;
	CamExtParam m_extParam;
};