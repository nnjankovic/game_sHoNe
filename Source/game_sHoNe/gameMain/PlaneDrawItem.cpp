#include "PlaneDrawItem.h"



PlaneDrawItem::PlaneDrawItem(std::shared_ptr<IRenderer> renderer) : DrawItem(renderer)
{
	//TODO: temporarly here remove hardcoded values from constructor
	m_properties.position.x = 0;
	m_properties.position.y = 0;
	m_properties.position.z = 0;
}


PlaneDrawItem::~PlaneDrawItem()
{
}


bool PlaneDrawItem::createShadersAndInputLayout()
{
	m_properties.vertexShaderByteCode = CompileShader(L"Shaders\\Shader.hlsl", nullptr, "VS", "vs_5_0");
	m_properties.pixelShaderByteCode = CompileShader(L"Shaders\\Shader.hlsl", nullptr, "PS", "ps_5_0");

	m_properties.inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return true;
}

bool PlaneDrawItem::loadGeometry()
{
	int n = 50;
	int m = 50;

	int numOfVertices = n*m;

	std::vector<Vertex> vertices;

	for (int i = 0; i<n; i++)
		for (int j = 0; j < m; j++)
		{
			float z = (n - 1) / 2 - i;
			float y = 0;
			float x = j - (m - 1) / 2;

			XMFLOAT4 color;

			if (i % 2)
				color = XMFLOAT4(Colors::Black);
			else
				color = XMFLOAT4(Colors::White);

			vertices.push_back(
				Vertex({ XMFLOAT3(x, y,z), color })
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

	m_geometry.vertices = vertices;
	m_geometry.indices = indices;

	m_geometry.VertexBufferSize = static_cast<uint16_t>(m_geometry.vertices.size()) * sizeof(Vertex);
	m_geometry.VertexByteStride = sizeof(Vertex);
	m_geometry.IndexBufferSize = static_cast<uint16_t>(m_geometry.indices.size()) * sizeof(uint16_t);

	return true;
}
