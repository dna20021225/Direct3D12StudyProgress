#include"TerrainActor.h"
#include"Game.h"

TerrainActor::TerrainActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName)
	: PlaneActor(game, shaderIndex, pos, textureName, 3000.0f, 3000.0f, 200.0f, 200.0f)
	, m_plane(UnitVecZ3d, pos)
{

}

TerrainActor::~TerrainActor()
{

}

void TerrainActor::update(float deltaTime)
{
	calcWorldTransform();
}

void TerrainActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!isEnabled())return;
	if (!isVisible())return;

	PlaneActor::draw(cmdList);
}