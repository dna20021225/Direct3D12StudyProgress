#include "BoxActor.h"
#include "Game.h"

BoxActor::BoxActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName, bool outer,
	float sx, float sy, float sz, MeshMaterial* material)
	: Actor(game, pos)
{
	m_mesh = std::make_unique<BoxMesh>(this, game->getRenderer(), shaderIndex,
		textureName, outer, sx, sy, sz, material);
}

BoxActor::BoxActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	std::vector<const wchar_t*> textureNames, bool outer,
	float sx, float sy, float sz, MeshMaterial* material)
	: Actor(game, pos)
{
	m_mesh = std::make_unique<BoxMesh>(this, game->getRenderer(), shaderIndex,
		textureNames, outer, sx, sy, sz, material);
}

BoxActor::~BoxActor()
{
}

void BoxActor::update(float deltaTime)
{	
	calcWorldTransform();
}

void BoxActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	m_mesh->draw(cmdList);
}

bool BoxActor::isEnabled() const
{
	return m_mesh->isEnabled();
}