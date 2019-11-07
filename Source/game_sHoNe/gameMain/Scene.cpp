#include "Scene.h"

#include "PlaneDrawItem.h"

Scene::Scene(HINSTANCE hInstance) : m_hInstance(hInstance)
{
	m_renderer = std::make_shared<D3D12Renderer>(hInstance);
}

void Scene::Init()
{
	m_renderer->Init();

	auto box = std::make_shared<BoxDrawItem>(m_renderer);
	box->create();
	m_Items.push_back(box);

/*	auto plane = std::make_shared<PlaneDrawItem>(m_renderer);
	plane->create();
	m_Items.push_back(plane);*/

	//For now this is hard coded will change when I add camera object and automatically update camera
	XMVECTOR pos = XMVectorSet(2.5f, 3.535f, 2.5f, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_viewMatrix, view);
}


void Scene::Draw()
{
	m_matrixStack = XMLoadFloat4x4(&m_viewMatrix);
	m_renderer->PrepareDraw();
	for (auto& item : m_Items)
		item->Draw(m_matrixStack);

	m_renderer->Present();
}

Scene::~Scene()
{
}
