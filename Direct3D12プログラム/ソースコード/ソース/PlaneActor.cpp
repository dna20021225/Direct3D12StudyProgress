#include "PlaneActor.h"
#include "Game.h"

PlaneActor::PlaneActor(Game* game,UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName,
	float sx, float sy, float sv, float su, MeshMaterial* material)
	: Actor(game, pos)
{
	m_mesh = std::make_unique<PlaneMesh>(this,
		game->getRenderer(), shaderIndex, textureName, sx, sy, su, sv, material);
}

PlaneActor::~PlaneActor()
{
}

void PlaneActor::update(float deltaTime)
{	
	calcWorldTransform();
}

void PlaneActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	m_mesh->draw(cmdList);
}

bool PlaneActor::isEnabled() const
{
	return m_mesh->isEnabled();
}