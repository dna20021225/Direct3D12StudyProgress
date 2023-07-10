#include "HemisphereMesh.h"
#include "GameUtil.h"
#include "Actor.h"
#include "Renderer.h"
#include <vector>

HemisphereMesh::HemisphereMesh(Actor* owner, Renderer* renderer, UINT shaderIndex,
	const wchar_t* textureName, bool outer, float radius, UINT nv, UINT nh,
	float imgWidth, float imgHeight, float cx, float cy, float imgRadius,
	float angleScale, bool orthographic, MeshMaterial* material)
	: Mesh(owner, renderer, shaderIndex, material)
	, m_indicesNum(0)
{
	nv = (nv >= 1) ? nv : 1;
	nh = (nh >= 3) ? nh : 3;

	XMMATRIX mat = XMMatrixIdentity();
	m_constIndex = renderer->allocateConstBuffer((void*)(&mat), sizeof(XMMATRIX));
	if (m_constIndex == -1) return;

	m_bufferIndex.resize(1);

	float cu = cx / imgWidth;
	float cv = cy / imgHeight;
	float radiusU = imgRadius / imgWidth;
	float radiusV = imgRadius / imgHeight;

	std::vector<VertexNorm> vertices;
	VertexNorm v;
	float angleV = XM_PIDIV2 / nv;
	float sign = (outer) ? -1.0f : 1.0f;
	float angleH = sign * XM_2PI / nh;
	for (UINT j = 0; j <= nv; ++j)
	{
		if (j == 0)
		{
			v = { { 0.0f, 0.0f, radius }, { cu, cv }, -1.0f * sign * UnitVecZ3d };
			vertices.push_back(v);
		}
		else
		{
			float rh = radius * XMScalarSin(j * angleV);
			float z = radius * XMScalarCos(j * angleV);
			for (UINT i = 0; i <= nh; ++i)
			{
				XMFLOAT3 pos = { rh * XMScalarCos(i * angleH), rh * XMScalarSin(i * angleH), z };
				XMFLOAT3 norm = -1.0f * sign * normalize(pos);
				float phi = (orthographic) ? j * angleV * angleScale : 0.5f * j * angleV * angleScale;
				float proj = (orthographic) ? XMScalarSin(phi) : XMScalarSin(phi) / XMScalarCos(phi);
				v = { pos,
					{ radiusU * proj * XMScalarCos(-(i * angleH)) + cu,
					  radiusV * proj * XMScalarSin(-(i * angleH)) + cv }, norm };
				vertices.push_back(v);
			}
		}
	}
	m_bufferIndex[0].vertexID = renderer->allocateVertexBuffer((void*)vertices.data(),
		sizeof(VertexNorm) * vertices.size(), sizeof(VertexNorm));
	if (m_bufferIndex[0].vertexID == -1) return;

	std::vector<unsigned short> indices;
	for (unsigned short i = 1; i <= nh; ++i)
	{
		indices.push_back(i);
		indices.push_back(0);
		indices.push_back(i + 1);
	}
	for (unsigned short i = 1; i <= (unsigned short)(nv - 1); ++i)
	{
		unsigned short baseV = 1 + (i - 1) * (nh + 1);
		for (unsigned short j = 1; j <= nh; ++j)
		{
			unsigned short baseH = baseV + j - 1;
			indices.push_back(baseH);
			indices.push_back(baseH + nh + 2);
			indices.push_back(baseH + nh + 1);
			indices.push_back(baseH);
			indices.push_back(baseH + 1);
			indices.push_back(baseH + nh + 2);
		}
	}
	m_indicesNum = (UINT)indices.size();
	m_bufferIndex[0].indexID = renderer->allocateIndexBuffer((void*)indices.data(),
		sizeof(unsigned short) * m_indicesNum);
	if (m_bufferIndex[0].indexID == -1) return;

	m_bufferIndex[0].shaderResourceID = renderer->allocateShaderResource(textureName);
	if (m_bufferIndex[0].shaderResourceID == -1) return;

	m_enabled = true;

	ConstData cdata;
	m_constIndex = renderer->allocateConstBuffer((void*)(&cdata), sizeof(ConstData));
}

HemisphereMesh::~HemisphereMesh()
{
}

void HemisphereMesh::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!m_enabled) return;

	XMMATRIX mat = m_owner->getWorldMatrix() * m_renderer->getCameraMatrix();
	m_renderer->uploadConstBuffer(m_constIndex, (void*)(&mat), sizeof(XMMATRIX));

	m_renderer->setShader(m_shaderIndex);
	m_renderer->setCommandShaderResourceView(0, m_bufferIndex[0].shaderResourceID);
	m_renderer->setCommandConstantBufferView(1, m_constIndex);
	m_renderer->setCommandVertexBufferView((UINT)m_bufferIndex[0].vertexID);
	m_renderer->setCommandIndexBufferView((UINT)m_bufferIndex[0].indexID);

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->DrawIndexedInstanced(m_indicesNum, 1, 0, 0, 0);

	ConstData cdata;
	setConstData(cdata);
	m_renderer->uploadConstBuffer(m_constIndex, (void*)(&cdata), sizeof(ConstData));
}