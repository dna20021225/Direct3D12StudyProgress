#include"FileMesh.h"
#include"Actor.h"
#include"Renderer.h"
#include<fstream>

FileMesh::FileMesh(Actor* owner, Renderer* renderer, UINT shaderIndex,
	const char* filePath, const char* imgFilePrefix, MeshMaterial* material)
	:Mesh(owner, renderer, shaderIndex, material)
{
	ConstData cdata;
	m_constIndex = renderer->allocateConstBuffer((void*)(&cdata), sizeof(ConstData));
	if (m_constIndex == -1)return;

	//XMMATRIX mat = XMMatrixIdentity();
	//m_constIndex = m_renderer->allocateConstBuffer((void*)(&mat), sizeof(XMMATRIX));
	
	std::vector<MeshData>meshData;
	if (!readGMeshFile(filePath, imgFilePrefix, meshData))return;

	m_bufferIndex.resize(meshData.size());
	m_indexNum.resize(meshData.size());
	for (size_t i = 0; i < meshData.size(); ++i)
	{
		m_bufferIndex[i].vertexID = m_renderer->allocateVertexBuffer((void*)meshData[i].vertices.data(),
			sizeof(VertexNorm) * meshData[i].vertices.size(), sizeof(VertexNorm));
			//sizeof(Vertex) * meshData[i].vertices.size(), sizeof(Vertex));
		if (m_bufferIndex[i].vertexID == -1)return;

		m_bufferIndex[i].indexID = m_renderer->allocateIndexBuffer((void*)meshData[i].indices.data(),
				sizeof(unsigned short) * meshData[i].indices.size());
		if (m_bufferIndex[i].indexID == -1)return;
		m_indexNum[i] = (int)meshData[i].indices.size();

		m_bufferIndex[i].shaderResourceID = m_renderer->allocateShaderResource(meshData[i].textureFilePath.c_str());
		if (m_bufferIndex[i].shaderResourceID == -1)return;
	}
	m_enabled = true;
}

FileMesh::~FileMesh()
{
}

void FileMesh::draw(ID3D12GraphicsCommandList* cmdList)
{
	if (!m_enabled)return;
	ConstData cdata;
	setConstData(cdata);
	m_renderer->uploadConstBuffer(m_constIndex, (void*)(&cdata), sizeof(ConstData));
	
	//XMMATRIX mat = m_owner->getWorldMatrix() * m_renderer->getCameraMatrix();
	//m_renderer->uploadConstBuffer(m_constIndex, (void*)(&mat), sizeof(XMMATRIX));

	m_renderer->setShader(m_shaderIndex);
	m_renderer->setCommandConstantBufferView(1, m_constIndex);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (size_t i = 0; i < m_bufferIndex.size(); ++i)
	{
		m_renderer->setCommandShaderResourceView(0, m_bufferIndex[i].shaderResourceID);
		m_renderer->setCommandVertexBufferView(m_bufferIndex[i].vertexID);
		m_renderer->setCommandIndexBufferView(m_bufferIndex[i].indexID);
		cmdList->DrawIndexedInstanced(m_indexNum[i], 1, 0, 0, 0);
	}
}

bool FileMesh::readGMeshFile(const char* filePath, const char* imgFilePrefix,
	std::vector<MeshData>& meshData)
{
	meshData.clear();
	std::ifstream fin(filePath, std::ios::in | std::ios::binary);
	if (!fin)return false;

	unsigned int meshNum;
	fin.read((char*)&meshNum, sizeof(unsigned int));
	if (meshNum == 0)return false;

	meshData.resize(meshNum);
	for (unsigned int i = 0; i < meshNum; ++i)
	{
		unsigned int vertexNum;
		unsigned int indexNum;
		fin.read((char*)&vertexNum, sizeof(unsigned int));
		fin.read((char*)&indexNum, sizeof(unsigned int));

		meshData[i].vertices.resize(vertexNum);
		meshData[i].indices.resize(indexNum);
		fin.read((char*)meshData[i].vertices.data(), sizeof(VertexNorm) * vertexNum);
		//fin.read((char*)meshData[i].vertices.data(), sizeof(Vertex) * vertexNum);
		fin.read((char*)meshData[i].indices.data(), sizeof(unsigned short) * indexNum);

		char textureName[256];
		fin.read(textureName, sizeof(char) * 256);
		std::string textureFilePath = textureName;
		textureFilePath = imgFilePrefix + textureFilePath;
		convertString(textureFilePath, meshData[i].textureFilePath);
	}
	fin.close();
	return true;
}

void FileMesh::convertString(std::string& src, std::wstring& dst)
{
	wchar_t* buff = new wchar_t[src.size() + 1];
	size_t cvt;
	mbstowcs_s(&cvt, buff, src.size() + 1, src.c_str(), _TRUNCATE);
	dst = buff;
	delete[]buff;
}