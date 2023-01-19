#pragma once

#include "D3D12Renderer.h"
#include "BoxDrawItem.h"
#include "UserControllerListener.h"
#include "CameraNode.h"
#include "Lights.h"

#include <memory>
#include <map>

class Scene
{
public:
	Scene(HINSTANCE hInstance, std::shared_ptr<UserControllerListener> userControlListener, std::shared_ptr<GameTimer> timer);

	void Init();
	void Exit() {};

	void setFpsOnWindow(std::wstring fps);

	void Draw();

	~Scene();

private:
	void UploadTextures();
	void BuildLights();
	void UploadLights();
	void BuildMaterials();
	void DrawAllItems();

private:
	std::vector<std::shared_ptr<Renderer3D::DrawItem>> m_Items;
	std::vector<std::shared_ptr<Renderer3D::DrawItem>> m_Mirrors;
	std::vector<std::shared_ptr<Renderer3D::DrawItem>> m_Reflections;
	std::vector<std::shared_ptr<Renderer3D::DrawItem>> m_Transparent;

	std::map<LightTypes::LightType, std::shared_ptr<LightTypes::Light>> m_lights;
	std::map<std::wstring, Renderer3D::Material> m_materials;
	std::shared_ptr<Renderer3D::IRenderer> m_renderer;
	HINSTANCE m_hInstance;
	
	std::shared_ptr<GameTimer> m_timer;
	std::shared_ptr<UserControllerListener> m_userControlListener;

	
	//XMFLOAT4X4 m_viewMatrix; //depends on camera position, angle, orientation
	XMMATRIX m_matrixStack;
	
	CameraNode m_camera;

	std::map<std::wstring ,Renderer3D::Texture> m_textures;
};

