#pragma once
#include <vector>
#include <unordered_map>
#include <array>
#include <string>
#include <DirectXMath.h>

#include "Structures.h"

class Mesh
{
private:
public:
	Mesh();
	Mesh(std::string id);
	~Mesh();

	bool Empty() const;
	void Clear();

	//Setters
	bool AddSubmesh(const std::string& id, Submesh& submesh);
	

	//Getters
	bool GetSubmesh(const std::string& submeshID, Submesh& submesh) const;
	const std::unordered_map<std::string, Submesh>& GetSubmeshMap() const;

	int GetIndiceSize() const;
	void AddIndice(int indice);
	void AddVertice(const Vertex& vertice);

	// Grouped Data
	const Vertex* GetVertexData() const;
	const UINT GetVertexByteWidth() const;
	const int* GetIndexData() const;
	const UINT GetIndexByteWidth() const;

private:
	//Container info
	std::vector<int> m_indiceList;
	std::vector<Vertex> m_verticeList;
	std::unordered_map<std::string, Submesh> m_submeshMap;

public: //D3D11 stuff
	void SetMatrix(const dx::XMMATRIX& matrix);
	const dx::XMMATRIX& GetMatrix() const;
private:
	dx::XMMATRIX m_matrix;


};