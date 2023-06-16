#include "BoxDrawItem.h"



BoxDrawItem::BoxDrawItem(std::shared_ptr<Renderer3D::IRenderer> renderer, float x, float y, float z, float width, float height, float depth):
	DrawItem(renderer),
	m_height(height),
	m_width(width),
	m_depth(depth)
{
	m_properties.position.x = x;
	m_properties.position.y = y;
	m_properties.position.z = z;
}

BoxDrawItem::~BoxDrawItem()
{

}

bool BoxDrawItem::loadGeometry()
{
	float w = 0.5f*m_width;
	float h = 0.5f*m_height;
	float d = 0.5f*m_depth;

	std::vector<Renderer3D::BasicVertex> vertices =
	{
		Renderer3D::BasicVertex({ XMFLOAT3(m_properties.position.x - w,
						  m_properties.position.y - h, 
						  m_properties.position.z - d),
						  XMFLOAT4(Colors::White) }),

		Renderer3D::BasicVertex({ XMFLOAT3(m_properties.position.x - w,
						  m_properties.position.y + h,
						  m_properties.position.z - d),
						  XMFLOAT4(Colors::Black) }),

		Renderer3D::BasicVertex({ XMFLOAT3(m_properties.position.x + w,
						  m_properties.position.y + h, 
						  m_properties.position.z - d), 
						  XMFLOAT4(Colors::Red) }),

		Renderer3D::BasicVertex({ XMFLOAT3(m_properties.position.x + w,
						  m_properties.position.y - h, 
						  m_properties.position.z - d), 
						  XMFLOAT4(Colors::Blue) }),

		Renderer3D::BasicVertex({ XMFLOAT3(m_properties.position.x - w,
						  m_properties.position.y - h, 
						  m_properties.position.z + d), 
						  XMFLOAT4(Colors::Turquoise) }),

		Renderer3D::BasicVertex({ XMFLOAT3(m_properties.position.x - w,
						  m_properties.position.y + h, 
						  m_properties.position.z + d), 
						  XMFLOAT4(Colors::Goldenrod) }),

		Renderer3D::BasicVertex({ XMFLOAT3(m_properties.position.x + w,
						  m_properties.position.y + h, 
						  m_properties.position.z + d), 
						  XMFLOAT4(Colors::Yellow) }),

		Renderer3D::BasicVertex({ XMFLOAT3(m_properties.position.x + w,
						  m_properties.position.y - h, 
						  m_properties.position.z + d), 
						  XMFLOAT4(Colors::Salmon) })
	};
	m_geometry.vertices = vertices;//.data();
	m_geometry.VertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(Renderer3D::BasicVertex);
	m_geometry.VertexByteStride = sizeof(Renderer3D::BasicVertex);

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

