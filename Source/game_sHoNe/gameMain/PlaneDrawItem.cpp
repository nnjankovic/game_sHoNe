#include "PlaneDrawItem.h"



PlaneDrawItem::PlaneDrawItem(std::shared_ptr<IRenderer> renderer, MathHelper::PositionVector position, float angle) : 
	DrawItem(renderer)
{
	m_properties.position = position;
}


PlaneDrawItem::~PlaneDrawItem()
{
}

bool PlaneDrawItem::loadGeometry()
{
	int n = 50;
	int m = 50;

	int numOfVertices = n*m;

	std::vector<BasicVertex> vertices;

	for (int i = 0; i<n; i++)
		for (int j = 0; j < m; j++)
		{
			float z = m_properties.position.z + (n - 1) / 2 - i;
			float y = m_properties.position.y + 0;
			float x = m_properties.position.x + j - (m - 1) / 2;

			XMFLOAT4 color;

			if (i % 2)
				color = XMFLOAT4(Colors::Black);
			else
				color = XMFLOAT4(Colors::White);

			vertices.push_back(
				BasicVertex({ XMFLOAT3(x, y,z), color })
			);
		}

	std::vector<std::uint16_t> indices;
	int numOfIndices = (n - 1)*(m - 1) * 2;

	for (std::uint16_t i = 0; i<n - 1; i++)
		for (std::uint16_t j = 0; j < m - 1; j++)
		{
			indices.push_back(i*n + j);
			indices.push_back(i*n + j + 1);
			indices.push_back((i + 1)*n + j);

			indices.push_back((i + 1)*n + j);
			indices.push_back(i*n + j + 1);
			indices.push_back((i + 1)*n + j + 1);
		}

	m_geometry.vertices = vertices;//.data();
	m_geometry.indices = indices;

	m_geometry.VertexBufferSize = static_cast<uint16_t>(vertices.size()) * sizeof(BasicVertex);
	m_geometry.VertexByteStride = sizeof(BasicVertex);
	m_geometry.IndexBufferSize = static_cast<uint16_t>(m_geometry.indices.size()) * sizeof(uint16_t);

	return true;
}
