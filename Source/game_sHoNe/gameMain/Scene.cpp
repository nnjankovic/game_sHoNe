#include "Scene.h"

#include "PlaneDrawItem.h"
#include "TexturedBox.h"
#include "TexturedPlane.h"
#include "LoadedDrawItem.h"
#include "MoveOnInputDrawItemComponent.h"

#include <filesystem>

Scene::Scene(HINSTANCE hInstance, std::shared_ptr<UserControllerListener> userControlListener, std::shared_ptr<GameTimer> timer) : 
	m_hInstance(hInstance),
	m_userControlListener(userControlListener),
	m_timer(timer),
	m_camera(m_userControlListener, m_timer)
{
	m_renderer = std::make_shared<Renderer3D::D3D12Renderer>(hInstance);
}

void Scene::Init()
{
	m_renderer->Init();

	UploadTextures();
	BuildMaterials();

	auto texBox1 = std::make_shared<TexturedBox>(m_renderer, -5, -15, 25, 2, 2, 2, m_textures[L"woodCrate"], m_materials[L"woodCrate"]);
	texBox1->create();
	m_Items.push_back(texBox1);
	/*
	auto texBox2 = std::make_shared<TexturedBox>(m_renderer, 0, 2.5, 14.5, 5, 5, 5, m_textures[L"woodCrate"], m_materials[L"woodCrate"]);
	auto moveComponent = std::make_shared<Renderer3D::MoveOnInputDrawItemComponent>(texBox2, [](Renderer3D::DrawItemProperties& properties, std::wstring button) {
		float inc = 0.7;
		if (button == L"I")
		{
			properties.position.z += inc;
		}
		else if (button == L"K")
		{
			properties.position.z -= inc;
		}
		else if (button == L"J")
		{
			properties.position.x -= inc;
		}
		else if (button == L"L")
		{
			properties.position.x += inc;
		}
	}, m_userControlListener);
	texBox2->addComponent(moveComponent);
	texBox2->create();
	m_Items.push_back(texBox2);

	auto reflectedTexBox2 = std::make_shared<TexturedBox>(m_renderer, 0, 2.5, 34.5, 5, 5, 5, m_textures[L"woodCrate"], m_materials[L"woodCrate"]);
	auto refMoveComponent = std::make_shared<Renderer3D::MoveOnInputDrawItemComponent>(reflectedTexBox2, [](Renderer3D::DrawItemProperties& properties, std::wstring button) {
		float inc = 0.7;
		if (button == L"I")
		{
			properties.position.z -= inc;
		}
		else if (button == L"K")
		{
			properties.position.z += inc;
		}
		else if (button == L"J")
		{
			properties.position.x -= inc;
		}
		else if (button == L"L")
		{
			properties.position.x += inc;
		}
	}, m_userControlListener);
	reflectedTexBox2->addComponent(refMoveComponent);
	reflectedTexBox2->create();
	m_Reflections.push_back(reflectedTexBox2);

	auto texBox3 = std::make_shared<TexturedBox>(m_renderer, 12.5, 2.5, 0, 5, 5, 5, m_textures[L"tile"], m_materials[L"tile"]);
	texBox3->create();
	m_Items.push_back(texBox3);

	auto texBox4 = std::make_shared<TexturedBox>(m_renderer, 7.5, 2.5, 0, 5, 5, 5, m_textures[L"tile"], m_materials[L"glass"]);
	texBox4->create();
	m_Items.push_back(texBox4);

	/*auto plane = std::make_shared<PlaneDrawItem>(m_renderer, MathHelper::PositionVector{0,0,0}, 0);
	plane->create();
	m_Items.push_back(plane);*/
	/*
	auto texturedPlane = std::make_shared<TexturedPlane>(m_renderer, MathHelper::Position3{ 0,0,0 }, 
														 50, 50, 0, 1, 
														 m_textures[L"checkboard"], MathHelper::Position2{6,6},
														 m_materials[L"stone"]);
	texturedPlane->create();
	m_Items.push_back(texturedPlane);

	auto reflectedTexturedPlane = std::make_shared<TexturedPlane>(m_renderer, MathHelper::Position3{ 0,0,49.5 },
		50, 50, 0, 1,
		m_textures[L"checkboard"], MathHelper::Position2{ 6,6 },
		m_materials[L"stone"]);
	reflectedTexturedPlane->create();
	m_Reflections.push_back(reflectedTexturedPlane); //TODO: Fix planes, need to be able to change position



	/*MathHelper::Position3 wallPosition{ 0, 24.5, 24 };
	int wallM = 50;
	int wallN = 25;
	auto texturedWallBack = std::make_shared<TexturedPlane>(m_renderer, wallPosition, wallM, wallN, 1.5708, m_textures[L"tile"], m_materials[L"stone"]);
	texturedWallBack->create();
	m_Items.push_back(texturedWallBack);
	auto texturedWallFront = std::make_shared<TexturedPlane>(m_renderer, wallPosition, wallM, wallN, -1.5708, m_textures[L"tile"], m_materials[L"stone"]);
	texturedWallFront->create();
	m_Items.push_back(texturedWallFront);*/
	/*
	auto mirror = std::make_shared<TexturedPlane>(m_renderer, MathHelper::Position3{ -18,13,24.4 },
		2, 2, -1.5708, 12,
		m_textures[L"ice"], MathHelper::Position2{ 1,1 },
		m_materials[L"mirror"],
		Renderer3D::ShaderType::Transparent);
	mirror->create();
	m_Mirrors.push_back(mirror);

	//auto loadPlane = std::make_shared<LoadedDrawItem>(m_renderer, L"Models\\plane1.obj", MathHelper::Position3{ 0, 7.5, 24.5}, -1.5708, 1, m_textures[L"tile"], m_materials[L"brick"]);
	//loadPlane->create();
	//m_Transparent.push_back(loadPlane);

	auto wireBox1 = std::make_shared<TexturedBox>(m_renderer, -5, 2.5, -5, 5, 5, 5, m_textures[L"wire"], m_materials[L"wire"], Renderer3D::ShaderType::AlphaTest);
	wireBox1->create();
	m_Transparent.push_back(wireBox1);

	/*auto transparentBox1 = std::make_shared<TexturedBox>(m_renderer, 0, 7.6, -5, 5, 5, 5, m_textures[L"ice"], m_materials[L"ice"], Renderer3D::ShaderType::Transparent);
	transparentBox1->create();
	m_Items.push_back(transparentBox1);*/

	
	//auto loadSphere = std::make_shared<LoadedDrawItem>(m_renderer, L"Models\\sphere.obj", MathHelper::Position3{ 0, 3, -10}, 0, 3, m_textures[L"ice"], m_materials[L"ice"], Renderer3D::ShaderType::Transparent);
	//loadSphere->create();
	//m_Transparent.push_back(loadSphere);

	BuildLights();

	std::vector<std::shared_ptr<Renderer3D::DrawItem>> allItems;
	allItems.reserve(m_Items.size() + m_Mirrors.size() + m_Transparent.size() + m_Reflections.size());
	allItems.insert(allItems.end(), m_Items.begin(), m_Items.end());
	allItems.insert(allItems.end(), m_Mirrors.begin(), m_Mirrors.end());
	allItems.insert(allItems.end(), m_Reflections.begin(), m_Reflections.end());
	allItems.insert(allItems.end(), m_Transparent.begin(), m_Transparent.end());
	m_renderer->UploadStaticGeometry(allItems);
}


void Scene::setFpsOnWindow(std::wstring fps)
{
	m_renderer->setWindowTitle(fps);
}

void Scene::Draw()
{
	//m_matrixStack = XMLoadFloat4x4(&m_camera.getViewMatrix());
	m_renderer->PrepareDraw();

	if (m_camera.isViewDirty()) {
		m_renderer->setCameraView(m_camera.getViewMatrix());
		m_renderer->setEyePosition(m_camera.getPosition());
	}

	UploadLights();

	DrawAllItems();

	m_renderer->Present();
}

Scene::~Scene()
{
}

void Scene::UploadTextures()
{
	std::filesystem::path texturesPath;
	if (std::filesystem::exists(L"..\\..\\Assets\\Textures"))
		texturesPath = L"..\\..\\Assets\\Textures";
	else if(std::filesystem::exists(L"..\\..\\..\\Assets\\Textures"))
		texturesPath = L"..\\..\\..\\Assets\\Textures";

	if (texturesPath.empty())
		return;

	Renderer3D::Texture woodCrateTexture;
	woodCrateTexture.name = L"woodCrate";
	woodCrateTexture.fileName = texturesPath.append("WoodCrate01.dds");
	woodCrateTexture.fileName = L"C:\\Users\\shone\\Documents\\GitHub\\game_sHoNe\\Assets\\Textures\\WoodCrate01.dds";
	m_renderer->UploadTexture(woodCrateTexture);
	m_textures[woodCrateTexture.name] = woodCrateTexture;
	texturesPath = texturesPath.parent_path();

	Renderer3D::Texture checkboardTexture;
	checkboardTexture.name = L"checkboard";
	checkboardTexture.fileName = texturesPath.append("checkboard.dds");
	m_renderer->UploadTexture(checkboardTexture);
	m_textures[checkboardTexture.name] = checkboardTexture;
	texturesPath = texturesPath.parent_path();

	Renderer3D::Texture tileTexture;
	tileTexture.name = L"tile";
	tileTexture.fileName = texturesPath.append("tile.dds");
	m_renderer->UploadTexture(tileTexture);
	m_textures[tileTexture.name] = tileTexture;
	texturesPath = texturesPath.parent_path();

	Renderer3D::Texture iceTexture;
	iceTexture.name = L"ice";
	iceTexture.fileName = texturesPath.append("ice.dds");
	m_renderer->UploadTexture(iceTexture);
	m_textures[iceTexture.name] = iceTexture;
	texturesPath = texturesPath.parent_path();

	Renderer3D::Texture wireTexture;
	wireTexture.name = L"wire";
	wireTexture.fileName = texturesPath.append("WireFence.dds");
	m_renderer->UploadTexture(wireTexture);
	m_textures[wireTexture.name] = wireTexture;
	texturesPath = texturesPath.parent_path();

	Renderer3D::Texture brick3Texture;
	brick3Texture.name = L"brick3";
	brick3Texture.fileName = texturesPath.append("bricks3.dds");
	m_renderer->UploadTexture(brick3Texture);
	m_textures[brick3Texture.name] = brick3Texture;
}

void Scene::BuildLights()
{
	
	LightTypes::LightParams directLightParams;
	directLightParams.lightType = LightTypes::LightType::DIRECTIONAL;
	directLightParams.Direction = { 0.57735f, -0.57735f, 0.57735f };
	directLightParams.Strength = { 0.5, 0.5, 0.5 };
	auto directLight = std::make_shared<LightTypes::Light>(m_userControlListener, directLightParams);
	m_lights[directLightParams.lightType] = directLight;

	LightTypes::LightParams pointLightParams;
	pointLightParams.lightType = LightTypes::LightType::POINT;
	pointLightParams.Strength = { 1, 1, 1 };
	pointLightParams.Position = { 0, 8, 0 };
	pointLightParams.FalloffEnd = 20;
	auto pointLight = std::make_shared<LightTypes::Light>(m_userControlListener, pointLightParams);
	m_lights[pointLightParams.lightType] = pointLight;

	
	/*LightTypes::LightParams spotLightParams;
	spotLightParams.lightType = LightTypes::LightType::SPOT;
	spotLightParams.Strength = { 1.0f, 1.0f, 1.0f };
	spotLightParams.Position = { 5, 8, 5 };
	spotLightParams.Direction = { 0, -1, 0 };
	spotLightParams.SpotPower = 30;
	spotLightParams.FalloffEnd = 30;
	auto spotLight = std::make_shared<LightTypes::Light>(m_userControlListener, spotLightParams);
	m_lights[spotLightParams.lightType] = spotLight;*/
}

void Scene::UploadLights()
{
	bool dirty = false;

	for (auto lightentry : m_lights)
		dirty |= lightentry.second->isDirty();

	if (!dirty)
		return;

	m_renderer->clearLights();
	for (auto lightentry : m_lights) {
		m_renderer->addLight(lightentry.second->getLightParams());
		lightentry.second->ClearDirtyFlag();
	}
}

void Scene::BuildMaterials()
{
	Renderer3D::Material bricks0;
	bricks0.name = L"brick";
	bricks0.index = 0;
	bricks0.materialConstants.diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0.materialConstants.fresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0.materialConstants.roughness = 0.1f;
	m_materials[bricks0.name] = bricks0;

	Renderer3D::Material stone0;
	stone0.name = L"stone";
	stone0.index = 1;
	stone0.materialConstants.diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	stone0.materialConstants.fresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone0.materialConstants.roughness = 0.3f;
	m_materials[stone0.name] = stone0;

	Renderer3D::Material tile0;
	tile0.name = L"tile";
	tile0.index = 2;
	tile0.materialConstants.diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	tile0.materialConstants.fresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0.materialConstants.roughness = 0.3f;
	m_materials[tile0.name] = tile0;

	Renderer3D::Material woodCrate;
	woodCrate.name = L"woodCrate";
	woodCrate.index = 3;
	woodCrate.materialConstants.diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	woodCrate.materialConstants.fresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	woodCrate.materialConstants.roughness = 0.2f;
	m_materials[woodCrate.name] = woodCrate;

	Renderer3D::Material glass;
	glass.name = L"glass";
	glass.index = 4;
	glass.materialConstants.diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	glass.materialConstants.fresnelR0 = XMFLOAT3(1.0f, 1.0f, 1.0f);
	glass.materialConstants.roughness = 0.99f;
	m_materials[glass.name] = glass;

	Renderer3D::Material ice;
	ice.name = L"ice";
	ice.index = 5;
	ice.materialConstants.diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.7f);
	ice.materialConstants.fresnelR0 = XMFLOAT3(1.0f, 1.0f, 1.0f);
	ice.materialConstants.roughness = 0.99f;
	m_materials[ice.name] = ice;

	Renderer3D::Material wire;
	wire.name = L"wire";
	wire.index = 6;
	wire.materialConstants.diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f);
	wire.materialConstants.fresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	wire.materialConstants.roughness = 0.25f;
	m_materials[wire.name] = wire;

	Renderer3D::Material mirror;
	mirror.name = L"mirror";
	mirror.index = 7;
	mirror.materialConstants.diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	mirror.materialConstants.fresnelR0 = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mirror.materialConstants.roughness = 0.99f;
	m_materials[mirror.name] = mirror;
}

void Scene::DrawAllItems()
{
	//TO DO: revisit matrixStack, maybe remove it
	for (auto& item : m_Items)
		item->Draw(m_matrixStack);

	for (auto& item : m_Mirrors)
	{
		item->changeShaderType(Renderer3D::ShaderType::Mirror);
		item->Draw(m_matrixStack);
	}

	for (auto& item : m_Reflections)
	{
		item->changeShaderType(Renderer3D::ShaderType::Reflection);
		item->Draw(m_matrixStack);
	}

	for (auto& item : m_Mirrors)
	{
		item->changeShaderType(Renderer3D::ShaderType::Transparent);
		item->Draw(m_matrixStack);
	}

	for (auto& item : m_Transparent)
	{
		item->Draw(m_matrixStack);		
	}
}
