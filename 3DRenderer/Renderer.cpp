#include "Renderer.h"
#include "Settings.h"

//Maybe works
//wWindow& Renderer::CreateWWindow(LPCWSTR windowName, HINSTANCE instance, UINT height, UINT width, int nCmdShow)
//{
//	std::function<void(MSG&)> func = std::bind(&Renderer::windowEventManager, this, std::placeholders::_1);
//	wWindow hwindow(windowName, instance, width, height, nCmdShow, func);
//	return hwindow;
//}

Renderer::Renderer()
{
	CreateDXCam();
}
Renderer::~Renderer(){}

//Window helper
wWindow Renderer::CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction)
{
	return wWindow(L"Renderer", instance, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, nCmdShow, eventFunction);
}


//Camera Manager
void Renderer::CreateDXCam()
{
	//Some default settings
	m_dxCam = Camera(0, 0, 0, 90, BASE_WINDOW_RATIO, 0.1f, 100.0f);
}
void Renderer::CreateDXCam(const DirectX::XMVECTOR& pos, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_dxCam = Camera(pos, fovDegrees, aspectRatio, nearZ, farZ);
}
void Renderer::CreateDXCam(float x, float y, float z, float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	m_dxCam = Camera(x, y, z, fovDegrees, aspectRatio, nearZ, farZ);
}
const Camera& Renderer::GetDXCamera() const { return m_dxCam; }
void Renderer::AddDXCamPos(float x, float y, float z)
{
	m_dxCam.AddPosition(x, y, z);
}

Mesh& Renderer::AddMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	//Skeleton unimplemented
	Mesh mesh(vertices, indices);
	m_models.push_back(mesh);
	return m_models[m_models.size() - 1];
}

const Mesh& Renderer::GetMesh(int index) const { return m_models[index]; }



