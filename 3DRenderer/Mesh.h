#pragma once
#include <vector>
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
	void SetIndiceCount(unsigned int count);
	void SetIndiceStartIndex(unsigned int count);
	void SetVerticeStartIndex(unsigned int count);

	void SetId(std::string id);
	void SetTextureId(std::string id);


	//Getters
	std::string GetId() const;
	std::string GetTextureId() const;
	unsigned int GetIndiceCount() const;

	unsigned int GetVerticeStartIndex() const;
	unsigned int GetIndiceStartIndex() const;

private:
	//Container info
	std::string m_id;

	/*    SUBMESH TODO            */
	std::string m_textureid;
	//Data
	unsigned int m_indiceCount;
	unsigned int m_indiceStartIndex;
	unsigned int m_verticeStartIndex;
	/*                            */
public: //D3D11 stuff
	void SetMatrix(const dx::XMMATRIX& matrix);
	const dx::XMMATRIX& GetMatrix() const;
private:
	dx::XMMATRIX m_matrix;


};