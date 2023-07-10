#include "BoxMesh.h"
#include "GameUtil.h"
#include "Actor.h"
#include "Renderer.h"

// テクスチャ単体
BoxMesh::BoxMesh(Actor* owner, Renderer* renderer, UINT shaderIndex,
	const wchar_t* textureName, bool outer,	float sx, float sy, float sz,
	MeshMaterial* material)
	: Mesh(owner, renderer, shaderIndex, material)
{
	m_bufferIndex.resize(1);

	ConstData cdata;
	m_constIndex = renderer->allocateConstBuffer((void*)(&cdata), sizeof(ConstData));

	if (!setVertex(outer, sx, sy, sz))return;

	if (!setIndex())return;

	m_bufferIndex[0].shaderResourceID = renderer->allocateShaderResource(textureName);
	if (m_bufferIndex[0].shaderResourceID == -1)return;

	m_enabled = true;
}

// 複数テクスチャ
BoxMesh::BoxMesh(Actor* owner, Renderer* renderer, UINT shaderIndex,
	std::vector<const wchar_t*>& textureNames, bool outer,
	float sx, float sy, float sz, MeshMaterial* material)
	: Mesh(owner, renderer, shaderIndex)
{
	if (textureNames.size() != 6)return;
	m_bufferIndex.resize(6);

	ConstData cdata;
	m_constIndex=renderer->allocateConstBuffer((void*)(&cdata), sizeof(ConstData));
	if (m_constIndex == -1)return;

	if (!setVertex(outer, sx, sy, sz))return;

	if (!setIndex())return;

	for (int i = 0; i < 6; ++i)
	{
		m_bufferIndex[i].shaderResourceID = 
			m_renderer->allocateShaderResource(textureNames[i]);
		if (m_bufferIndex[i].shaderResourceID == -1)return;
	}

	m_enabled = true;
}

BoxMesh::~BoxMesh()
{

}

void BoxMesh::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!m_enabled)return;

	ConstData cdata;
	setConstData(cdata);
	m_renderer->uploadConstBuffer(m_constIndex, (void*)(&cdata), sizeof(ConstData));

	m_renderer->setShader(m_shaderIndex);
	m_renderer->setCommandConstantBufferView(1, m_constIndex);
	m_renderer->setCommandVertexBufferView((UINT)m_bufferIndex[0].vertexID);
	m_renderer->setCommandIndexBufferView((UINT)m_bufferIndex[0].indexID);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	if (m_bufferIndex.size()==6)
	{
		for (int i = 0; i < 6; ++i)
		{
			m_renderer->setCommandShaderResourceView(0,
				m_bufferIndex[i].shaderResourceID);
			cmdList->DrawIndexedInstanced(6, 1, i * 6, 0, 0);
		}
	}
	else
	{
		m_renderer->setCommandShaderResourceView(0, 
			m_bufferIndex[0].shaderResourceID);
		cmdList->DrawIndexedInstanced(36, 1, 0, 0, 0);
	}
}

bool BoxMesh::setVertex(bool outer, float sx, float sy, float sz)
{
	VertexNorm vertices[24] = {};
	float sign = (outer) ? 1.0f : -1.0f;
	// x-front
	XMFLOAT3 norm = outer * UnitVecX3d;
	vertices[0] = { { 0.5f * sx, sign *  0.5f * sy, -0.5f * sz }, { 0.0f, 1.0f } , norm };
	vertices[1] = { { 0.5f * sx, sign *  0.5f * sy,  0.5f * sz }, { 0.0f, 0.0f } , norm };
	vertices[2] = { { 0.5f * sx, sign * -0.5f * sy, -0.5f * sz }, { 1.0f, 1.0f } , norm };
	vertices[3] = { { 0.5f * sx, sign * -0.5f * sy,  0.5f * sz }, { 1.0f, 0.0f } , norm };

	// y-front
	norm = outer * UnitVecY3d;
	vertices[4] = { { sign * -0.5f * sx, 0.5f * sy, -0.5f * sz }, { 0.0f, 1.0f } , norm };
	vertices[5] = { { sign * -0.5f * sx, 0.5f * sy,  0.5f * sz }, { 0.0f, 0.0f } , norm };
	vertices[6] = { { sign * 0.5f * sx,  0.5f * sy, -0.5f * sz }, { 1.0f, 1.0f } , norm };
	vertices[7] = { { sign * 0.5f * sx,  0.5f * sy,  0.5f * sz }, { 1.0f, 0.0f } , norm };

	// x-back
	norm = -1.0f * outer * UnitVecX3d;
	vertices[8] =  { { -0.5f * sx, sign * -0.5f * sy, -0.5f * sz }, { 0.0f, 1.0f } , norm };
	vertices[9] =  { { -0.5f * sx, sign * -0.5f * sy,  0.5f * sz }, { 0.0f, 0.0f } , norm };
	vertices[10] = { { -0.5f * sx, sign *  0.5f * sy, -0.5f * sz }, { 1.0f, 1.0f } , norm };
	vertices[11] = { { -0.5f * sx, sign *  0.5f * sy,  0.5f * sz }, { 1.0f, 0.0f } , norm };

	// y-back
	vertices[12] = { { sign *  0.5f * sx, -0.5f * sy, -0.5f * sz }, { 0.0f, 1.0f } , norm };
	vertices[13] = { { sign *  0.5f * sx, -0.5f * sy,  0.5f * sz }, { 0.0f, 0.0f } , norm };
	vertices[14] = { { sign * -0.5f * sx, -0.5f * sy, -0.5f * sz }, { 1.0f, 1.0f } , norm };
	vertices[15] = { { sign * -0.5f * sx, -0.5f * sy,  0.5f * sz }, { 1.0f, 0.0f } , norm };

	// z-front
	vertices[16] = { {  0.5f * sx, sign *  0.5f * sy, 0.5f * sz }, { 0.0f, 1.0f } , norm };
	vertices[17] = { { -0.5f * sx, sign *  0.5f * sy, 0.5f * sz }, { 0.0f, 0.0f } , norm };
	vertices[18] = { {  0.5f * sx, sign * -0.5f * sy, 0.5f * sz }, { 1.0f, 1.0f } , norm };
	vertices[19] = { { -0.5f * sx, sign * -0.5f * sy, 0.5f * sz }, { 1.0f, 0.0f } , norm };

	// z-back
	vertices[20] = { { -0.5f * sx, sign *  0.5f * sy, -0.5f * sz }, { 0.0f, 1.0f } , norm };
	vertices[21] = { {  0.5f * sx, sign *  0.5f * sy, -0.5f * sz }, { 0.0f, 0.0f } , norm };
	vertices[22] = { { -0.5f * sx, sign * -0.5f * sy, -0.5f * sz }, { 1.0f, 1.0f } , norm };
	vertices[23] = { {  0.5f * sx, sign * -0.5f * sy, -0.5f * sz }, { 1.0f, 0.0f } , norm };

	m_bufferIndex[0].vertexID = m_renderer->allocateVertexBuffer((void*)vertices,
		sizeof(vertices), sizeof(VertexNorm));

	return (m_bufferIndex[0].vertexID == -1) ? false : true;
}

bool BoxMesh::setIndex()
{
	unsigned short indices[36] = {};
	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 3; indices[4] = 2; indices[5] = 1;

	indices[6] = 4; indices[7] = 5; indices[8] = 6;
	indices[9] = 7; indices[10] = 6; indices[11] = 5;

	indices[12] = 8; indices[13] = 9; indices[14] = 10;
	indices[15] = 11; indices[16] = 10; indices[17] = 9;

	indices[18] = 12; indices[19] = 13; indices[20] = 14;
	indices[21] = 15; indices[22] = 14; indices[23] = 13;

	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 19; indices[28] = 18; indices[29] = 17;

	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 23; indices[34] = 22; indices[35] = 21;

	m_bufferIndex[0].indexID = 
		m_renderer->allocateIndexBuffer((void*)indices, sizeof(indices));

	return (m_bufferIndex[0].indexID == -1) ? false : true;
}