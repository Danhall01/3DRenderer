#pragma once

#include <map> // RBTree
#include <unordered_map> //HashMap
#include <string>
#include "Mesh.h"
#include "Texture.h"


struct Vertex
{
	std::array<float,3> position;
	std::array<float,3> normal;
	std::array<float,2> uv;
};


class Assets
{
public:
	Assets();
	~Assets();
	bool ParseFromObjFile(std::string path, std::string filename, bool severity);
	bool ParseFromMtlFile(std::string path, bool severity);
	//bool ParseImageFromFile(std::string path); //Todo once I get there

	
	const Texture GetTexture(std::string texId) const;
	const Mesh GetMesh(std::string meshId) const;
	const std::map<std::string, Mesh> GetMeshMap() const;

	const std::vector<int>& GetIndiceVector() const;
	const std::vector<Vertex>& GetVertexVector() const;
private:
	static inline void InitCMD();
	//Converters from stream to array
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
	std::map<std::string, Mesh> m_meshMap;
	std::unordered_map<std::string, Texture> m_textureMap;
	std::unordered_map<std::string, unsigned char*> m_images;
};

