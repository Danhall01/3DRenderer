#pragma once

#include <map> // RBTree
#include <unordered_map> //HashMap
#include <string>

#include <d3d11.h>
#include <wrl\client.h>
using namespace Microsoft;
#include "Structures.h"

#include "Mesh.h"
#include "Texture.h"



class Assets
{
public:
	Assets();
	~Assets();
	bool ParseFromObjFile(std::string path, std::string filename, bool severity);
	bool ParseFromMtlFile(std::string path, bool severity);
	//bool ParseImageFromFile(std::string path); //Todo once I get there

	
	const Texture GetTexture(std::string texId) const;
	bool GetMesh(std::string meshId, Mesh& mesh) const;
	const std::unordered_map<std::string, Mesh> GetMeshMap() const;

	const std::vector<int>& GetIndiceVector() const;
	const std::vector<Vertex>& GetVertexVector() const;

	const Vertex* GetVertexData() const;
	const UINT GetVertexByteWidth() const;
	const int* GetIndexData() const;
	const UINT GetIndexByteWidth() const;


	// TODO: Create: Copy | Move | Add :operators
private:
	std::array<float, 3> MakeFXYZ(std::istringstream& pstream);
	std::array<float, 2> MakeFXY(std::istringstream& pstream);
	//Helper functions
	void AddMesh(Mesh mesh, int indiceSize);


private:
	std::vector<int> m_indiceList;
	std::vector<Vertex> m_verticeList;



	// Data for the Parser, ensures no duplicates
	std::unordered_map<std::string, int> m_indiceMap;
	int m_indiceMapIter = 0;

	// 
	std::unordered_map<std::string, Mesh> m_meshMap;
	std::unordered_map<std::string, Texture> m_textureMap;
	std::unordered_map<std::string, unsigned char*> m_images;
};

