#include "PlaneMesh.h"
#include "GameUtil.h"
#include "Actor.h"
#include "Renderer.h"

PlaneMesh::PlaneMesh(Actor* owner, Renderer* renderer, UINT shaderIndex,
	const wchar_t* textureName, float sx, float sy, float su, float sv,
	MeshMaterial* material)
	: Mesh(owner, renderer, shaderIndex, material)
{
	ConstData cdata;
	m_constIndex = renderer->allocateConstBuffer((void*)(&cdata), sizeof(ConstData));

	BufferIndex buffer;

	VertexNorm vertices[4];
	XMFLOAT3 norm = UnitVecZ3d;
	vertices[0] = { {-0.5f * sx, -0.5f * sy, 0.0f},{0.0f,   sv} , norm };
	vertices[1] = { { 0.5f * sx, -0.5f * sy, 0.0f},{0.0f, 0.0f} , norm };
	vertices[2] = { {-0.5f * sx,  0.5f * sy, 0.0f},{  su,   sv} , norm };
	vertices[3] = { { 0.5f * sx,  0.5f * sy, 0.0f},{  su, 0.0f} , norm };

	buffer.vertexID = renderer->allocateVertexBuffer((void*)vertices,
		sizeof(vertices), sizeof(VertexNorm));
	if (buffer.vertexID == -1)return;
	buffer.shaderResourceID = renderer->allocateShaderResource(textureName);
	if (buffer.shaderResourceID == -1)
	{
		releaseBuffer(buffer);
		return;
	}
	m_bufferIndex.push_back(buffer);

	m_enabled = true;
}

PlaneMesh::~PlaneMesh()
{

}

void PlaneMesh::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!m_enabled)return;

	ConstData cdata;
	setConstData(cdata);
	m_renderer->uploadConstBuffer(m_constIndex, (void*)(&cdata), sizeof(ConstData));

	m_renderer->setShader(m_shaderIndex);
	m_renderer->setCommandShaderResourceView(0,
		(UINT)m_bufferIndex[0].shaderResourceID);
	m_renderer->setCommandConstantBufferView(1, (UINT)m_constIndex);
	m_renderer->setCommandVertexBufferView((UINT)m_bufferIndex[0].vertexID);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	cmdList->DrawInstanced(4, 1, 0, 0);
}