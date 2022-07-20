#pragma once

#include <map> // RBTree
#include <unordered_map> //HashMap
#include <string>

#include <d3d11.h>
#include <wrl\client.h>
using namespace Microsoft;

#include "Mesh.h"
#include "Structures.h"



class Assets
{
public:
	Assets();
	~Assets();
	bool ParseFromObjFile(std::string path, std::string filename, bool severity);
	bool ParseFromMtlFile(std::string path, bool severity);
	//bool ParseImageFromFile(std::string path); //Todo once I get there

	
	bool GetMesh(std::string meshId, Mesh& mesh) const;
	const std::unordered_map<std::string, Mesh> GetMeshMap() const;

	//TODO
	bool GetTexture(std::string texId, Texture& texture) const;
	const std::unordered_map<std::string, Texture> GetTextureMap() const;

	// TODO
	//bool GetImage(std::string texId, unsigned char*& img) const;
	//const std::unordered_map<std::string, Texture> GetImageMap() const;

	// TODO: Create: Copy | Move | Add :operators
private:
	std::array<float, 3> MakeFXYZ(std::istringstream& pstream);
	std::array<float, 2> MakeFXY(std::istringstream& pstream);
	//Helper functions
	inline void AddMesh(const std::string& meshID, Mesh& mesh);
	void ParseMesh(Mesh& mesh,
		const std::vector<std::string>& indiceStrVec,
		const std::unordered_map<std::string, int>& indexCountMap,
		const std::vector<std::array<float, 3>>& posList,
		const std::vector<std::array<float, 3>>& normalList,
		const std::vector<std::array<float, 2>>& uvList);

private:
	// Core Asset Data
	std::unordered_map<std::string, Mesh> m_meshMap;
	std::unordered_map<std::string, Texture> m_textureMap;
	std::unordered_map<std::string, unsigned char*> m_images;
};

