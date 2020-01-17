#include "BoxDrawItem.h"



BoxDrawItem::BoxDrawItem(std::shared_ptr<IRenderer> renderer, float x, float y, float z, float width, float height, float depth): 
	DrawItem(renderer),
	m_height(height),
	m_width(width),
	m_depth(depth)
{
	//TODO: temporarly here remove hardcoded values from constructor
	m_properties.position.x = x;
	m_properties.position.y = y;
	m_properties.position.z = z;
}

BoxDrawItem::~BoxDrawItem()
{

}

bool BoxDrawItem::createShadersAndInputLayout()
{
	m_properties.vertexShaderByteCode = CompileShader(L"Shaders\\Shader.hlsl", nullptr, "VS", "vs_5_0");
	m_properties.pixelShaderByteCode =	CompileShader(L"Shaders\\Shader.hlsl", nullptr, "PS", "ps_5_0");

	m_properties.inputLayout = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return true;
}

bool BoxDrawItem::loadGeometry()
{
	float w = 0.5*m_width;
	float h = 0.5*m_height;
	float d = 0.5*m_depth;

	std::vector<Vertex> vertices =
	{
		Vertex({ XMFLOAT3(m_properties.position.x - w,
						  m_properties.position.y - h, 
						  m_properties.position.z - d),
						  XMFLOAT4(Colors::White) }),

		Vertex({ XMFLOAT3(m_properties.position.x - w, 
						  m_properties.position.y + h,
						  m_properties.position.z - d),
						  XMFLOAT4(Colors::Black) }),

		Vertex({ XMFLOAT3(m_properties.position.x + w,
						  m_properties.position.y + h, 
						  m_properties.position.z - d), 
						  XMFLOAT4(Colors::Red) }),

		Vertex({ XMFLOAT3(m_properties.position.x + w, 
						  m_properties.position.y - h, 
						  m_properties.position.z - d), 
						  XMFLOAT4(Colors::Blue) }),

		Vertex({ XMFLOAT3(m_properties.position.x - w, 
						  m_properties.position.y - h, 
						  m_properties.position.z + d), 
						  XMFLOAT4(Colors::Turquoise) }),

		Vertex({ XMFLOAT3(m_properties.position.x - w, 
						  m_properties.position.y + h, 
						  m_properties.position.z + d), 
						  XMFLOAT4(Colors::Goldenrod) }),

		Vertex({ XMFLOAT3(m_properties.position.x + w, 
						  m_properties.position.y + h, 
						  m_properties.position.z + d), 
						  XMFLOAT4(Colors::Yellow) }),

		Vertex({ XMFLOAT3(m_properties.position.x + w, 
						  m_properties.position.y - h, 
						  m_properties.position.z + d), 
						  XMFLOAT4(Colors::Salmon) })
	};
	m_geometry.vertices = vertices;//.data();
	m_geometry.VertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(Vertex);
	m_geometry.VertexByteStride = sizeof(Vertex);

	m_geometry.indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	m_geometry.IndexBufferSize = static_cast<UINT>(m_geometry.indices.size()) * sizeof(uint16_t);

	return true;
}

