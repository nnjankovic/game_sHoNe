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
	m_renderer = std::make_shared<D3D12Renderer>(hInstance);
}

void Scene::Init()
{
	m_renderer->Init();

	UploadTextures();

	/*auto box1 = std::make_shared<BoxDrawItem>(m_renderer, 0, 5, 0);
	box1->create();
	m_Items.push_back(box1);

	auto box2 = std::make_shared<BoxDrawItem>(m_renderer, 5, 3, 0, 4, 4, 4);
	box2->create();
	m_Items.push_back(box2);

	auto box3 = std::make_shared<BoxDrawItem>(m_renderer, 0, 3, 5);
	box3->create();
	m_Items.push_back(box3);

	auto box4 = std::make_shared<BoxDrawItem>(m_renderer, -5, 6, 0, 2, 2, 2);
	box4->create();
	m_Items.push_back(box4);*/

	auto texBox1 = std::make_shared<TexturedBox>(m_renderer, -5, 0, 1, 2, 2, 2, m_textures[L"woodCrate"]);
	texBox1->create();
	m_Items.push_back(texBox1);

	auto texBox2 = std::make_shared<TexturedBox>(m_renderer, 0, 0, 2.5, 5, 5, 5, m_textures[L"woodCrate"]);
	texBox2->create();
	m_Items.push_back(texBox2);

	/*auto plane = std::make_shared<PlaneDrawItem>(m_renderer, MathHelper::PositionVector{0,0,0}, 0);
	plane->create();
	m_Items.push_back(plane);*/

	auto texturedPlane = std::make_shared<TexturedPlane>(m_renderer, MathHelper::PositionVector{ 0,0,0 }, 0, m_textures[L"checkboard"]);
	texturedPlane->create();
	m_Items.push_back(texturedPlane);

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
	if(m_camera.isViewDirty())
		m_renderer->setCameraView(m_camera.getViewMatrix());
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
	Texture woodCrateTexture;
	woodCrateTexture.name = L"woodCrate";
	woodCrateTexture.fileName = L"..\\..\\..\\Assets\\Textures\\WoodCrate01.dds";
	m_renderer->UploadTexture(woodCrateTexture);
	m_textures[woodCrateTexture.name] = woodCrateTexture;

	Texture checkboardTexture;
	checkboardTexture.name = L"checkboard";
	checkboardTexture.fileName = L"..\\..\\..\\Assets\\Textures\\checkboard.dds";
	m_renderer->UploadTexture(checkboardTexture);
	m_textures[checkboardTexture.name] = checkboardTexture;
}
