#pragma once
#include "Structures.h"
#include <vector>

struct ParticleData
{
	float position[3];
};

class Particle
{
public: // Methods
	Particle();
	~Particle();

	HRESULT Init(ID3D11Device* device, std::vector<ParticleData>& data, UINT particleCount);

	// Getters
	ID3D11ShaderResourceView* GetSRVP() const;
	ID3D11ShaderResourceView*const* GetSRVPP() const;
	ID3D11UnorderedAccessView* GetUAVP() const;
	ID3D11UnorderedAccessView*const* GetUAVPP() const;

	ID3D11VertexShader* GetVertexShaderP() const;
	ID3D11VertexShader*const* GetVertexShaderPP() const;

	ID3D11GeometryShader* GetGeometryShaderP() const;
	ID3D11GeometryShader*const* GetGeometryShaderPP() const;

	ID3D11PixelShader* GetPixelShaderP() const;
	ID3D11PixelShader*const* GetPixelShaderPP() const;

	ID3D11ComputeShader* GetComputeShaderP() const;
	ID3D11ComputeShader*const* GetComputeShaderPP() const;

	UINT Count() const;
	bool IsInitialized() const;
private:
	HRESULT BuildBuffer(ID3D11Device* device, std::vector<ParticleData>& data, UINT particleCount);
	HRESULT BuildSRV(ID3D11Device* device, UINT particleCount);
	HRESULT BuildUAV(ID3D11Device* device, UINT particleCount);

	HRESULT CompileShaders(ID3D11Device* device);

public:	// Variables
private:
	UINT m_particles;
	bool m_initialized = false;

	// Data
	WRL::ComPtr<ID3D11Buffer>              m_buffer;
	WRL::ComPtr<ID3D11ShaderResourceView>  m_srv;
	WRL::ComPtr<ID3D11UnorderedAccessView> m_uav;
	// Shaders
	WRL::ComPtr<ID3D11VertexShader>        m_vShader;
	WRL::ComPtr<ID3D11GeometryShader>      m_gShader;
	WRL::ComPtr<ID3D11PixelShader>         m_pShader;
	WRL::ComPtr<ID3D11ComputeShader>       m_cShader;
};

