#include "TexturedBox.h"



TexturedBox::TexturedBox(std::shared_ptr<IRenderer> renderer, float x, float y, float z, float width, float height, float depth, Texture tex) :
	DrawItem(renderer),
	m_height(height),
	m_width(width),
	m_depth(depth)
{
	m_properties.position.x = x;
	m_properties.position.y = y;
	m_properties.position.z = z;
	m_properties.texture = tex;

	m_properties.isTextured = true;
}


TexturedBox::~TexturedBox()
{
}

bool TexturedBox::createShadersAndInputLayout()
{
	m_properties.vertexShaderByteCode = CompileShader(L"Shaders\\TexturedShader.hlsl", nullptr, "VS", "vs_5_0");
	m_properties.pixelShaderByteCode  = CompileShader(L"Shaders\\TexturedShader.hlsl", nullptr, "PS", "ps_5_0");

	m_properties.inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return true;
}

bool TexturedBox::loadGeometry()
{
	float w = 0.5*m_width;
	float h = 0.5*m_height;
	float d = 0.5*m_depth;

	std::vector<Vertex> vertices(24);

	//FRONT
	vertices[0]  = Vertex(-w, +h, -d, 0, 0);
	vertices[1]  = Vertex(+w, +h, -d, 1, 0);
	vertices[2]  = Vertex(-w, -h, -d, 0, 1);
	vertices[3]  = Vertex(+w, -h, -d, 1, 1);

	//RIGHT
	vertices[4]  = Vertex(+w, +h, -d, 0, 0);
	vertices[5]  = Vertex(+w, +h, +d, 1, 0);
	vertices[6]  = Vertex(+w, -h, -d, 0, 1);
	vertices[7]  = Vertex(+w, -h, +d, 1, 1);

	//LEFT
	vertices[8]  = Vertex(-w, +h, +d, 0, 0);
	vertices[9]  = Vertex(-w, +h, -d, 1, 0);
	vertices[10] = Vertex(-w, -h, +d, 0, 1);
	vertices[11] = Vertex(-w, -h, -d, 1, 1);

	//BACK
	vertices[12] = Vertex(+w, +h, +d, 0, 0);
	vertices[13] = Vertex(-w, +h, +d, 1, 0);
	vertices[14] = Vertex(+w, -h, +d, 0, 1);
	vertices[15] = Vertex(-w, -h, +d, 1, 1);

	//TOP
	vertices[16] = Vertex(-w, +h, +d, 0, 0);
	vertices[17] = Vertex(+w, +h, +d, 1, 0);
	vertices[18] = Vertex(-w, +h, -d, 0, 1);
	vertices[19] = Vertex(+w, +h, -d, 1, 1);

	//BOTTOM
	vertices[20] = Vertex(-w, -h, +d, 0, 0);
	vertices[21] = Vertex(+w, -h, +d, 1, 0);
	vertices[22] = Vertex(-w, -h, -d, 0, 1);
	vertices[23] = Vertex(+w, -h, -d, 1, 1);

	m_geometry.vertices = vertices;
	m_geometry.VertexBufferSize = static_cast<UINT>(m_geometry.vertices.size()) * sizeof(Vertex);
	m_geometry.VertexByteStride = sizeof(Vertex);

	m_geometry.indices = {
		//FRONT
		0, 1, 2,
		1, 3, 2,

		//RIGHT
		4, 5, 6,
		5, 7, 6,

		//LEFT
		8, 9, 10,
		9, 11, 10,

		//BACK
		12, 13, 14,
		13, 15, 14,

		//TOP
		16, 17, 18,
		17, 19, 18,

		//BOTTOM
		20, 21, 22,
		21, 23, 22		
	};
	m_geometry.IndexBufferSize = static_cast<UINT>(m_geometry.indices.size()) * sizeof(uint16_t);

	return false;
}
