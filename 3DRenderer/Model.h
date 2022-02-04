#pragma once
#include <vector>
#include <DirectXMath.h>


struct Vertex 
{//Weakness, does not manage Colors
	float position[4];
	float normal[4];
	float UV[2];
	
	float padding[2];
};
struct Mesh
{
	unsigned int materialIndex;
	unsigned int textureIndex;
	unsigned int vertexIndex;
	
	std::vector<int> indicies;
};
struct Material
{
	float ka;
	float kd;
	float ks;
	float padding;
};
//structs for CBuffers (In case any come up)

class Model
{
public:
	Model();
	~Model();

	const Material& GetMaterial(unsigned int index) const;
	const Mesh& GetMesh(unsigned int index, unsigned int& startPos) const;
	const Vertex*& GetVertexData() const;
	const DirectX::XMMATRIX& GetTransformMatrix() const;

	void AddVerticies(std::vector<Vertex> newVerticies, unsigned int* objectSize);
	void SetTransformMatrix(DirectX::XMMATRIX transform);
	
	void LoadMesh(std::vector<int> indicies, unsigned int materialNr, unsigned int textureNr, unsigned int startIndex);
	void LoadMesh(Mesh newMesh, unsigned int materialNr);

	void LoadMaterial(float ka, float kd, float ds);
	void LoadMaterial(Material material);


private:



private:
	std::vector<Mesh> _MeshVec;
	std::vector<Material> _MaterialVec;

	std::vector<int> _VertexObjectSize; // [a, a+b=c, c+d=e, ...]

	std::vector<Vertex> _VertexVec;
	DirectX::XMMATRIX _TransformMatrix;
};

