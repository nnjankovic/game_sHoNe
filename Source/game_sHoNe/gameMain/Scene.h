#pragma once

#include "D3D12Renderer.h"
#include "BoxDrawItem.h"

#include <memory>

class Scene
{
public:
	Scene(HINSTANCE hInstance);

	void Init();
	void Exit() {};

	void Draw();

	~Scene();

private:
	std::vector<std::shared_ptr<DrawItem>> m_Items;
	std::shared_ptr<IRenderer> m_renderer;
	HINSTANCE m_hInstance;

	XMFLOAT4X4 m_viewMatrix; //depends on camera position, angle, orientation
	XMMATRIX m_matrixStack;
};

