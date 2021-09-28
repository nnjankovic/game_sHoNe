#include "Scene.h"

#include "PlaneDrawItem.h"
#include "TexturedBox.h"
#include "TexturedPlane.h"

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

	auto texBox1 = std::make_shared<TexturedBox>(m_renderer, -5, 0, 1, 2, 2, 2, m_textures[L"woodCrate"], m_materials[L"woodCrate"]);
	texBox1->create();
	m_Items.push_back(texBox1);

	auto texBox2 = std::make_shared<TexturedBox>(m_renderer, 0, 0, 2.5, 5, 5, 5, m_textures[L"woodCrate"], m_materials[L"woodCrate"]);
	texBox2->create();
	m_Items.push_back(texBox2);

	auto texBox3 = std::make_shared<TexturedBox>(m_renderer, 10, 2.5, 0, 5, 5, 5, m_textures[L"tile"], m_materials[L"tile"]);
	texBox3->create();
	m_Items.push_back(texBox3);

	auto texBox4 = std::make_shared<TexturedBox>(m_renderer, 7.5, 2.5, 0, 5, 5, 5, m_textures[L"tile"], m_materials[L"glass"]);
	texBox4->create();
	m_Items.push_back(texBox4);

	/*auto plane = std::make_shared<PlaneDrawItem>(m_renderer, MathHelper::PositionVector{0,0,0}, 0);
	plane->create();
	m_Items.push_back(plane);*/

	auto texturedPlane = std::make_shared<TexturedPlane>(m_renderer, MathHelper::PositionVector{ 0,0,0 }, 0, m_textures[L"checkboard"], m_materials[L"stone"]);
	texturedPlane->create();
	m_Items.push_back(texturedPlane);

	BuildLights();

	m_renderer->UploadStaticGeometry(m_Items);
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

	for (auto& item : m_Items)
		//TO DO: revisit matrixStack, maybe remove it
		item->Draw(m_matrixStack);

	m_renderer->Present();
}

Scene::~Scene()
{
}

void Scene::UploadTextures()
{
	Renderer3D::Texture woodCrateTexture;
	woodCrateTexture.name = L"woodCrate";
	woodCrateTexture.fileName = L"C:\\Users\\nenad.n.jankovic\\Documents\\GitHub\\game_sHoNe\\Assets\\Textures\\WoodCrate01.dds";
	m_renderer->UploadTexture(woodCrateTexture);
	m_textures[woodCrateTexture.name] = woodCrateTexture;

	Renderer3D::Texture checkboardTexture;
	checkboardTexture.name = L"checkboard";
	checkboardTexture.fileName = L"C:\\Users\\nenad.n.jankovic\\Documents\\GitHub\\game_sHoNe\\Assets\\Textures\\checkboard.dds";
	m_renderer->UploadTexture(checkboardTexture);
	m_textures[checkboardTexture.name] = checkboardTexture;

	Renderer3D::Texture tileTexture;
	tileTexture.name = L"tile";
	tileTexture.fileName = L"C:\\Users\\nenad.n.jankovic\\Documents\\GitHub\\game_sHoNe\\Assets\\Textures\\tile.dds";
	m_renderer->UploadTexture(tileTexture);
	m_textures[tileTexture.name] = tileTexture;
}

void Scene::BuildLights()
{
	
	/*LightTypes::LightParams directLightParams;
	directLightParams.lightType = LightTypes::LightType::DIRECTIONAL;
	directLightParams.Direction = { 0.57735f, -0.57735f, 0.57735f };
	directLightParams.Strength = { 1, 1, 1 };
	auto directLight = std::make_shared<LightTypes::Light>(m_userControlListener, directLightParams);
	m_lights[directLightParams.lightType] = directLight;*/

	LightTypes::LightParams pointLightParams;
	pointLightParams.lightType = LightTypes::LightType::POINT;
	pointLightParams.Strength = { 1, 1, 0.8 };
	pointLightParams.Position = { 0, 8, 0 };
	pointLightParams.FalloffEnd = 30;
	auto pointLight = std::make_shared<LightTypes::Light>(m_userControlListener, pointLightParams);
	m_lights[pointLightParams.lightType] = pointLight;

	
	/*LightTypes::LightParams spotLightParams;
	spotLightParams.lightType = LightTypes::LightType::SPOT;
	spotLightParams.Strength = { 0.0f, 0.0f, 1.0f };
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
	glass.materialConstants.roughness = 1.0f;
	m_materials[glass.name] = glass;
}