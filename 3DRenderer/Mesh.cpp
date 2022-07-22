#include "Mesh.h"

Mesh::Mesh()
{
	m_matrix = {};
}
Mesh::Mesh(std::string id)
{
	m_matrix = {};
}
Mesh::~Mesh(){}

bool Mesh::Empty() const
{
	return (
		m_indiceList.empty() &&
		m_verticeList.empty() &&
		m_submeshMap.empty()
		);
}
void Mesh::Clear()
{
	m_indiceList.clear();
	m_verticeList.clear();
	m_submeshMap.clear();

	m_matrix = {};
}


void Mesh::SetMatrix(const dx::XMMATRIX& matrix)
{
	m_matrix = matrix;
}
const dx::XMMATRIX& Mesh::GetMatrix() const
{
	return m_matrix;
}


bool Mesh::AddSubmesh(const std::string& id, Submesh& submesh)
{
	return m_submeshMap.try_emplace(id, submesh).second;
}
bool Mesh::GetSubmesh(const std::string& submeshID, Submesh& submesh) const
{
	if (m_submeshMap.count(submeshID) > 0)
	{
		submesh = m_submeshMap.at(submeshID);
		return true;
	}
	return false;
}
const std::unordered_map<std::string, Submesh>& Mesh::GetSubmeshMap() const
{
	return m_submeshMap;
}

int Mesh::GetIndiceSize() const
{
	return static_cast<int>(m_indiceList.size());
}
void Mesh::AddIndice(int indice)
{
	m_indiceList.push_back(indice);
}
void Mesh::AddVertice(const Vertex& vertice)
{
	m_verticeList.push_back(vertice);
}


// Grouped Data
const Vertex* Mesh::GetVertexData() const
{
	return m_verticeList.data();
}
const UINT Mesh::GetVertexByteWidth() const
{
	return (UINT)m_verticeList.size() * sizeof(Vertex);
}
const int* Mesh::GetIndexData() const
{
	return m_indiceList.data();
}
const UINT Mesh::GetIndexByteWidth() const
{
	return (UINT)m_indiceList.size() * sizeof(int);
}
