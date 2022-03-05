#include "Model.h"

Mesh::Mesh(){}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) :
	m_verticies(vertices), m_indices(indices)
{}

Mesh::~Mesh(){}

void Mesh::SetVertices(std::vector<Vertex> vertices)
{
	m_verticies = vertices;
}

void Mesh::AddVertices(std::vector<Vertex> newVertices)
{
	//Requires newVertices to be a copy rather than a reference
	m_verticies.insert(std::end(m_verticies), std::begin(newVertices), std::end(newVertices));
}

void Mesh::SetIndices(std::vector<unsigned int> indices)
{
	m_indices  = indices;
}

void Mesh::AddIndices(std::vector<unsigned int> newIndices)
{
	//Requires newIndices to be a copy rather than a reference
	m_indices.insert(std::end(m_indices), std::begin(newIndices), std::end(newIndices));
}

void Mesh::AddSubMesh(unsigned int startIndex, unsigned int indicesCount)
{
	SubMesh s;
	s.m_indicesCount = indicesCount;
	s.m_startIndex = startIndex;
	m_subMeshVec.push_back(s);
}

void Mesh::ClearSubmeshes()
{
	m_subMeshVec.clear();
}

void Mesh::Clear()
{
	m_subMeshVec.clear();
	m_indices.clear();
	m_verticies.clear();
}
