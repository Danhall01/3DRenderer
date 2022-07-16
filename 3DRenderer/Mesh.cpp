#include "Mesh.h"

Mesh::Mesh()
{
	m_indiceCount = 0;
	m_matrix = {};
	m_verticeStartIndex = 0;
	m_indiceStartIndex = 0;
}
Mesh::Mesh(std::string id)
{
	this->SetId(id);
	m_indiceCount = 0;
	m_matrix = {};
	m_verticeStartIndex = 0;
	m_indiceStartIndex = 0;
}
Mesh::~Mesh(){}

bool Mesh::Empty() const
{
	return (
		m_id.empty()
		);
}
void Mesh::Clear()
{
	m_id.clear();
	m_textureid.clear();

	m_verticeStartIndex = 0;
	m_indiceStartIndex = 0;
	m_indiceCount = 0;
	m_matrix = {};
}

void Mesh::SetIndiceCount(unsigned int count)
{
	m_indiceCount = count;
}
void Mesh::SetVerticeStartIndex(unsigned int count)
{
	m_verticeStartIndex = count;
}
void Mesh::SetIndiceStartIndex(unsigned int count)
{
	m_indiceStartIndex = count;
}


void Mesh::SetId(std::string id)
{
	m_id = id; 
}
void Mesh::SetTextureId(std::string id)
{ 
	m_textureid = id; 
}

std::string Mesh::GetId() const
{
	if (m_id.empty())
	{
		return "Error";
	}
	return m_id;
}
std::string Mesh::GetTextureId() const
{
	if (m_textureid.empty())
	{
		return "Error";
	}
	return m_textureid;
}

unsigned int Mesh::GetIndiceCount() const
{
	return m_indiceCount;
}

unsigned int Mesh::GetIndiceStartIndex() const
{
	return m_indiceStartIndex;
}
unsigned int Mesh::GetVerticeStartIndex() const
{
	return m_verticeStartIndex;
}


void Mesh::SetMatrix(const dx::XMMATRIX& matrix)
{
	m_matrix = matrix;
}
const dx::XMMATRIX& Mesh::GetMatrix() const
{
	return m_matrix;
}