#pragma once
#include "wWindow.h"
#include "Model.h"


#include <vector>
#include <string>
#include <wrl/client.h>



class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	wWindow CreateWWindow(HINSTANCE instance, int nCmdShow, std::function<void(MSG&)> eventFunction);


	//void AddModel(std::string objPath);
	//void RemoveModel(int index);
	//void ClearModel();

	//bool SetCBuffer(UINT buffer, void* change);

	//void Render();
	//void Clear();

private:
	

private:
	//std::vector<Model> m_modelVec;
};