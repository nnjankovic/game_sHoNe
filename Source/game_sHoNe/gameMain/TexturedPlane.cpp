#include "TexturedPlane.h"

TexturedPlane::TexturedPlane(std::shared_ptr<IRenderer> renderer, MathHelper::PositionVector position, float angle, Texture tex) :
	DrawItem(renderer)
{
	m_properties.position = position;
	m_properties.isTextured = true;
	m_properties.texture = tex;
}

bool TexturedPlane::createShadersAndInputLayout()
{
	m_properties.vertexShaderByteCode = CompileShader(L"Shaders\\TexturedShader.hlsl", nullptr, "VS", "vs_5_0");
	m_properties.pixelShaderByteCode = CompileShader(L"Shaders\\TexturedShader.hlsl", nullptr, "PS", "ps_5_0");

	m_properties.inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return true;
}

bool TexturedPlane::loadGeometry()
{
	int n = 50;
	int m = 50;

	int numOfVertices = n * m;

	std::vector<TexturedVertex> vertices;

	for (float i = 0; i < n; i++)
		for (float j = 0; j < m; j++)
		{
			float z = m_properties.position.z + (n - 1) / 2 - i;
			float y = m_properties.position.y + 0;
			float x = m_properties.position.x + j - (m - 1) / 2;

			XMFLOAT4 color;

			float texX = (i/n)*7 <= 1.0001 ? (i / n) * 7 : (i / n) * 7 - 1;
			float texY = ((j / m) * 7) <= 1.0001 ? ((j / m) * 7) : ((j / m) * 7) - 1;

			vertices.push_back(
				TexturedVertex(x, y, z, texX, texY )
			);
		}

	std::vector<std::uint16_t> indices;
	int numOfIndices = (n - 1)*(m - 1) * 2;

	for (std::uint16_t i = 0; i < n - 1; i++)
		for (std::uint16_t j = 0; j < m - 1; j++)
		{
			indices.push_back(i*n + j);
			indices.push_back(i*n + j + 1);
			indices.push_back((i + 1)*n + j);

			indices.push_back((i + 1)*n + j);
			indices.push_back(i*n + j + 1);
			indices.push_back((i + 1)*n + j + 1);
		}

	m_geometry.texVertices = vertices;
	m_geometry.indices = indices;

	m_geometry.VertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(TexturedVertex);
	m_geometry.VertexByteStride = sizeof(TexturedVertex);
	m_geometry.IndexBufferSize = static_cast<uint16_t>(m_geometry.indices.size()) * sizeof(uint16_t);

	return true;
}
