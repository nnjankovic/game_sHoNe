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

	m_properties.shaderType = ShaderType::TEXTURED;

	m_properties.xAngle = 0;
}


TexturedBox::~TexturedBox()
{
}

bool TexturedBox::loadGeometry()
{
	float w = 0.5*m_width;
	float h = 0.5*m_height;
	float d = 0.5*m_depth;

	std::vector<TexturedVertex> vertices(24);

	//FRONT
	vertices[0] =  TexturedVertex(m_properties.position.x -w, m_properties.position.z +h, m_properties.position.z -d, 0, 0);
	vertices[1]  = TexturedVertex(m_properties.position.x +w, m_properties.position.z +h, m_properties.position.z -d, 1, 0);
	vertices[2]  = TexturedVertex(m_properties.position.x -w, m_properties.position.z -h, m_properties.position.z -d, 0, 1);
	vertices[3]  = TexturedVertex(m_properties.position.x +w, m_properties.position.z -h, m_properties.position.z -d, 1, 1);

	//RIGHT
	vertices[4]  = TexturedVertex(m_properties.position.x +w, m_properties.position.z +h, m_properties.position.z -d, 0, 0);
	vertices[5]  = TexturedVertex(m_properties.position.x +w, m_properties.position.z +h, m_properties.position.z +d, 1, 0);
	vertices[6]  = TexturedVertex(m_properties.position.x +w, m_properties.position.z -h, m_properties.position.z -d, 0, 1);
	vertices[7]  = TexturedVertex(m_properties.position.x +w, m_properties.position.z -h, m_properties.position.z +d, 1, 1);

	//LEFT
	vertices[8]  = TexturedVertex(m_properties.position.x -w, m_properties.position.z +h, m_properties.position.z +d, 0, 0);
	vertices[9]  = TexturedVertex(m_properties.position.x -w, m_properties.position.z +h, m_properties.position.z -d, 1, 0);
	vertices[10] = TexturedVertex(m_properties.position.x -w, m_properties.position.z -h, m_properties.position.z +d, 0, 1);
	vertices[11] = TexturedVertex(m_properties.position.x -w, m_properties.position.z -h, m_properties.position.z -d, 1, 1);

	//BACK
	vertices[12] = TexturedVertex(m_properties.position.x +w, m_properties.position.z +h, m_properties.position.z +d, 0, 0);
	vertices[13] = TexturedVertex(m_properties.position.x -w, m_properties.position.z +h, m_properties.position.z +d, 1, 0);
	vertices[14] = TexturedVertex(m_properties.position.x +w, m_properties.position.z -h, m_properties.position.z +d, 0, 1);
	vertices[15] = TexturedVertex(m_properties.position.x -w, m_properties.position.z -h, m_properties.position.z +d, 1, 1);

	//TOP
	vertices[16] = TexturedVertex(m_properties.position.x -w, m_properties.position.z +h, m_properties.position.z +d, 0, 0);
	vertices[17] = TexturedVertex(m_properties.position.x +w, m_properties.position.z +h, m_properties.position.z +d, 1, 0);
	vertices[18] = TexturedVertex(m_properties.position.x -w, m_properties.position.z +h, m_properties.position.z -d, 0, 1);
	vertices[19] = TexturedVertex(m_properties.position.x +w, m_properties.position.z +h, m_properties.position.z -d, 1, 1);

	//BOTTOM
	vertices[20] = TexturedVertex(m_properties.position.x -w, m_properties.position.z -h, m_properties.position.z +d, 0, 0);
	vertices[21] = TexturedVertex(m_properties.position.x +w, m_properties.position.z -h, m_properties.position.z +d, 1, 0);
	vertices[22] = TexturedVertex(m_properties.position.x -w, m_properties.position.z -h, m_properties.position.z -d, 0, 1);
	vertices[23] = TexturedVertex(m_properties.position.x +w, m_properties.position.z -h, m_properties.position.z -d, 1, 1);

	m_geometry.texVertices = vertices;//.data();
	m_geometry.VertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(TexturedVertex);
	m_geometry.VertexByteStride = sizeof(TexturedVertex);

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
		20, 22, 21,
		21, 22, 23		
	};
	m_geometry.IndexBufferSize = static_cast<UINT>(m_geometry.indices.size()) * sizeof(uint16_t);

	return false;
}
