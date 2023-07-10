#include "Mesh.h"
#include "Actor.h"
#include "Renderer.h"

Mesh::Mesh(Actor* owner, Renderer* renderer, UINT shaderIndex,
	MeshMaterial* material)
	: m_enabled(false)
	, m_constIndex(-1)
	, m_owner(owner)
	, m_renderer(renderer)
	, m_shaderIndex(shaderIndex)
{
	if (material)
	{
		m_oriMaterial = *material;
		m_curMaterial = *material;
	}
}

Mesh::~Mesh()
{
	if (m_constIndex != -1)
	{
		m_renderer->releaseConstBuffer(m_constIndex);
		m_constIndex = -1;
	}
	for (auto it = m_bufferIndex.begin(); it != m_bufferIndex.end(); ++it)
	{
		releaseBuffer(*it);
	}
	m_bufferIndex.clear();
}

void Mesh::releaseBuffer(BufferIndex& buffer)
{
	if (buffer.vertexID != -1)
	{
		m_renderer->releaseVertexBuffer(buffer.vertexID);
		buffer.vertexID = -1;
	}
	if (buffer.indexID != -1)
	{
		m_renderer->releaseIndexBuffer(buffer.indexID);
	}
	buffer.shaderResourceID = -1;
}

void Mesh::setConstData(ConstData& cdata)
{
	XMFLOAT3 eye = m_renderer->getCameraExtParam().eye;
	cdata.eye = XMFLOAT4(eye.x, eye.y, eye.z, 1.0);
	cdata.light = m_renderer->getLight();
	cdata.material = m_curMaterial;
	cdata.world = m_owner->getWorldMatrix();
	cdata.viewProj = m_renderer->getCameraMatrix();
}

void Mesh::setCurrentMaterial(MeshMaterial material)
{
	m_curMaterial = material;
}

void Mesh::setOriginalMaterial(MeshMaterial material)
{
	m_curMaterial = material;
	m_oriMaterial = material;
}

void Mesh::resetMaterial()
{
	m_curMaterial = m_oriMaterial;
}