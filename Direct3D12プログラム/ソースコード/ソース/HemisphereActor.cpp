#include"HemisphereActor.h"
#include"Game.h"

HemisphereActor::HemisphereActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const wchar_t* textureName, bool outer, float radius, UINT nv, UINT nh,
	float imgWidth, float imgHeight, float cx, float cy, float imgRadius,
	float angleScale, bool orthographic, MeshMaterial* material)
	:Actor(game, pos)
{
	m_mesh = std::make_unique<HemisphereMesh>(this, game->getRenderer(),
		shaderIndex, textureName, outer, radius, nv, nh, imgWidth, imgHeight,
		cx, cy, imgRadius, angleScale, orthographic, material);
}

HemisphereActor::~HemisphereActor()
{

}

void HemisphereActor::update(float deltaTime)
{
	calcWorldTransform();
}

void HemisphereActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	m_mesh->draw(cmdList);
}

bool HemisphereActor::isEnabled()const
{
	return m_mesh->isEnabled();
}