#include"BlockActor.h"
#include"Game.h"

BlockActor::BlockActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName, XMFLOAT3 size)
	:BoxActor(game, shaderIndex, XMFLOAT3(pos.x, pos.y, 0.5f * size.z), textureName, true,
		size.x, size.y, size.z)
{
	setOriginalMaterial(MeshMaterial(XMFLOAT3(0.6f, 0.3f, 0.0f),
		XMFLOAT3(0.08, 0.08, 0.08), 1.0f, XMFLOAT3(0.3f, 0.3f, 0.3f)));

	m_box = AABBCollision(this, size, ZeroVec3d);
}

BlockActor::~BlockActor()
{

}

void BlockActor::update(float deltaTime)
{
	calcWorldTransform();
}

void BlockActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isEnabled())return;
	if (!isVisible())return;

	BoxActor::draw(cmdList);
}