#include"SphereActor.h"
#include"Game.h"

SphereActor::SphereActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName, bool outer, float radius,
	UINT nv, UINT nh, float su, float sv, MeshMaterial* material)
	:Actor(game, pos)
{
	m_mesh = std::make_unique<SphereMesh>(this, game->getRenderer(),
		shaderIndex, textureName, outer, radius, nv, nh, su, sv,
		material);
}

SphereActor::~SphereActor()
{

}

void SphereActor::update(float deltaTime)
{
	calcWorldTransform();
}

void SphereActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	m_mesh->draw(cmdList);
}

bool SphereActor::isEnabled()const
{
	return m_mesh->isEnabled();
}