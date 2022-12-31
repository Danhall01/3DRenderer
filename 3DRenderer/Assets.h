#pragma once

#include <map> // RBTree
#include <unordered_map> //HashMap
#include <string>

#include "Mesh.h"
#include "Texture.h"
#include "quadTree.h"
#include "Structures.h"

#include "enables.h"


class Assets
{
public:
	Assets();
	~Assets();
	bool ParseFromObjFile(std::string path, std::string filename, bool severity);
	bool ParseFromMtlFile(std::string path, std::string filename, bool severity);

	// Getters
	bool GetMesh(std::string meshId, Mesh& mesh) const;
	const std::unordered_map<std::string, Mesh>& GetMeshMap() const;

	bool GetTextureData(const std::string& texId, TextureData& texture) const;
	bool GetTexture(const std::string& texId, Texture& texture) const;
	const std::unordered_map<std::string, Texture>& GetTextureMap() const;

	bool GetImage(std::string imgId, Image& img) const;
	const std::unordered_map<std::string, Image>& GetImageMap() const;


	// Adders
	bool AddTexture(std::string texID, const Texture& tex);

	// Clears the image map preemptively: OBS! Unless a copy is saved, image allocations will be lost
	void Clear();
	

	// Frustum Culling
	void BuildFrustumCulling(std::vector< std::pair<std::string, dx::XMMATRIX> >& drawTargets, float maxHeight, float minHeight, float multiplier);
	std::vector< std::pair<std::string, dx::XMMATRIX> > FrustumCull(dx::BoundingFrustum frustum) const;


private:
	std::array<float, 3> MakeFXYZ(std::istringstream& pstream);
	std::array<float, 2> MakeFXY(std::istringstream& pstream);
	//Helper functions
	inline void AddMesh(const std::string& meshID, Mesh& mesh);
	void AddBoundedBox(const std::string& meshID, const float smallestPoint[3], const float biggestPoint[3]);
	
	inline void ParseMesh(Mesh& mesh,const std::vector<std::string>&indiceStrVec, const std::unordered_map<std::string, int>& indexCountMap, const std::vector<std::array<float, 3>>& posList, const std::vector<std::array<float, 3>>& normalList, const std::vector<std::array<float, 2>>& uvList);
	inline void LoadToImage(const std::string& path, const std::string& filename);




private:
	// Core Asset Data
	std::unordered_map<std::string, Mesh>        m_meshMap;
	std::unordered_map<std::string, dx::BoundingBox> m_boundedBoxes;

	quadTree<std::pair<std::string, dx::XMMATRIX>> m_tree;

	std::unordered_map<std::string, Texture>     m_textureMap;
	std::unordered_map<std::string, Image>       m_images;
};

