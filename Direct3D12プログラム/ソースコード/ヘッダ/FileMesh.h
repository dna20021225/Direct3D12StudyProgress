#pragma once

#include "Mesh.h"
#include <string>
#include "GameUtil.h"

struct MeshData
{
	std::vector<VertexNorm> vertices;
	//std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
	std::wstring textureFilePath;
};

class FileMesh : public Mesh
{
public:
	FileMesh(class Actor* owner, class Renderer* renderer, UINT shaderIndex,
		const char* filePath, const char* imgFilePrefix, MeshMaterial* material = nullptr);
	~FileMesh();

	void draw(ID3D12GraphicsCommandList* cmdList) override;

protected:
	bool readGMeshFile(const char* filePath, const char* imgFilePrefix,
		std::vector<MeshData>& meshData);
	void convertString(std::string& src, std::wstring& dst);

	std::vector<unsigned int> m_indexNum;
};
