#include "TexturedPlane.h"

using namespace Renderer3D;

//TODO: revisit constructors, bad design
TexturedPlane::TexturedPlane(std::shared_ptr<IRenderer> renderer, MathHelper::Position3 position,
	int sizeM, int sizeN, float angle, int scaling,
	const Renderer3D::Texture& tex, MathHelper::Position2 textureScale,
	const Renderer3D::Material& mat, Renderer3D::ShaderType shaderType) :
	m_m(sizeM), m_n(sizeN),
	DrawItem(renderer)
{
	m_properties.position = position;
	m_properties.shaderType = shaderType;
	m_properties.texture = tex;
	m_properties.material = mat;
	m_properties.xAngle = angle;
	m_properties.scaleFactor = scaling;
	m_properties.textureScaling = textureScale;
}

TexturedPlane::~TexturedPlane()
{
}


bool TexturedPlane::loadGeometry()
{
	int n = m_n;
	int m = m_m;

	int numOfVertices = n * m;

	std::vector<TexturedVertex> vertices;

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	for (float i = 0; i < n; i++)
		for (float j = 0; j < m; j++)
		{
			float z = /*m_properties.position.z +*/ (n - 1) / 2 - i;
			float y = /*m_properties.position.y + */0;
			float x = /*m_properties.position.x +*/ j - (m - 1) / 2;

			//float texX = (i/n)*7 <= 1.0001 ? (i / n) * 7 : (i / n) * 7 - 1;
			//float texY = ((j / m) * 7) <= 1.0001 ? ((j / m) * 7) : ((j / m) * 7) - 1;
			float texX = i * du;
			float texY = j * dv;

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

	XMStoreFloat4x4(&m_properties.objectConstants.TexTransform,
		XMMatrixScaling(m_properties.textureScaling.x, m_properties.textureScaling.y, 1.0f));

	return true;
}
