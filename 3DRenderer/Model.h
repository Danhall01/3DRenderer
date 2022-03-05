#pragma once
#include <vector>
#include <DirectXMath.h>


struct Vertex 
{
	float position[4];
	float normal[4];
	float UV[2];
	
	float padding[2];
};



class Mesh
{
private:
	struct SubMesh
	{
		unsigned int m_startIndex;
		unsigned int m_indicesCount;
	};
public:
	Mesh();
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	~Mesh();

	void SetVertices(std::vector<Vertex> vertices);
	void AddVertices(std::vector<Vertex> newVertices);

	void SetIndices(std::vector<unsigned int> indices);	
	void AddIndices(std::vector<unsigned int> newIndices);

	void AddSubMesh(unsigned int startIndex, unsigned int indicesCount);
	void ClearSubmeshes();

	void Clear();

private:



private:
	std::vector<SubMesh> m_subMeshVec;
	std::vector<Vertex> m_verticies;
	std::vector<unsigned int> m_indices;
};

//class Mesh : public Mesh
//{
//public:
//
//
//private:
//
//
//private:
//
//	
//};
//class Skeleton : public Mesh
//{
//public:
//
//
//private:
//
//
//private:
//
//
//};