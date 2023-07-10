#include"FileMeshActor.h"
#include"Game.h"

FileMeshActor::FileMeshActor(Game* game, UINT shaderIndex, XMFLOAT3 pos,
	const char* fliePath, const char* imgFilePrefix, MeshMaterial* material)
	:Actor(game, pos)
{
	m_mesh = std::make_unique<FileMesh>(this, game->getRenderer(),
		shaderIndex, fliePath, imgFilePrefix, material);
}

FileMeshActor::~FileMeshActor()
{

}

void FileMeshActor::update(float deltatime)
{
	calcWorldTransform();
}

void FileMeshActor::draw(ID3D12GraphicsCommandList* cmdList)
{
	m_mesh->draw(cmdList);
}

bool FileMeshActor::isEnabled()const
{
	return m_mesh->isEnabled();
}