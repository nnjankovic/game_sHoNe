#include "LoadedDrawItem.h"

#include <fstream>
#include <sstream>
#include <set>

using namespace Renderer3D;

LoadedDrawItem::LoadedDrawItem(std::shared_ptr<Renderer3D::IRenderer> renderer, std::wstring path,
	MathHelper::Position3 position, float angle, float scaleFactor,
	const Renderer3D::Texture & tex, const Renderer3D::Material & mat,
	Renderer3D::ShaderType shaderType) :
	DrawItem(renderer), m_modelPath(path)
{
	m_properties.position = position;
	m_properties.shaderType = shaderType;
	m_properties.texture = tex;
	m_properties.material = mat;
	m_properties.xAngle = angle;
	m_properties.scaleFactor = scaleFactor;
}

bool LoadedDrawItem::loadGeometry()
{
	std::ifstream fin(m_modelPath);

	if (!fin)
	{
		assert(false);
		return false;
	}

	std::vector<MathHelper::Position3> temp_vertices;
	std::vector<MathHelper::Position3> temp_normals;
	std::vector<MathHelper::Position2> temp_texPos;

	struct valuePair {
		int index;
		std::set<int> faceNormalsIndexes;
	};
	std::map<std::string, valuePair> fmap;

	std::vector<TexturedVertex> vertices;
	std::vector<std::uint16_t> indicies;

	std::string s;
	int findex = 0;
	while (std::getline(fin, s)) {
		std::stringstream ss(s);
		std::string type;
		ss >> type;

		if (type.compare("v") == 0)
		{
			MathHelper::Position3 pos;
			ss >> pos.x >> pos.y >> pos.z;
			temp_vertices.push_back(pos);
		}
		else if (type.compare("vt") == 0)
		{
			MathHelper::Position2 pos;
			ss >> pos.x >> pos.y;
			temp_texPos.push_back(pos);
		}
		else if (type.compare("vn") == 0)
		{
			MathHelper::Position3 pos;
			ss >> pos.x >> pos.y >> pos.z;
			temp_normals.push_back(pos);
		}
		else if (type.compare("f") == 0)
		{
			for (int i = 0; i < 3; i++)
			{
				std::string fs;
				ss >> fs;

				int vertexIndex = 0;
				int texIndex = 0;
				int normalIndex = 0;
				sscanf_s(fs.c_str(), "%d/%d/%d", &vertexIndex, &texIndex, &normalIndex);

				fs = fs.substr(0, fs.find_last_of('/'));

				if (fmap.count(fs) == 0)
				{
					valuePair val;
					val.index = findex++;
					val.faceNormalsIndexes.insert(normalIndex);
					fmap[fs] = val;

					TexturedVertex vertex(temp_vertices[vertexIndex - 1], temp_texPos[texIndex - 1], temp_normals[normalIndex - 1]);
					vertices.push_back(vertex);
				}
				else if(fmap[fs].faceNormalsIndexes.count(normalIndex) == 0)
				{
					fmap[fs].faceNormalsIndexes.insert(normalIndex);
					vertices[fmap[fs].index].Normal.x += temp_normals[normalIndex - 1].x;
					vertices[fmap[fs].index].Normal.y += temp_normals[normalIndex - 1].y;
					vertices[fmap[fs].index].Normal.z += temp_normals[normalIndex - 1].z;
				}
				else
				{
					int i = 2;
				}
				indicies.push_back(fmap[fs].index);
			}
		}

	}
	fin.close();

	for (auto &vertex : vertices)
		vertex.Normal = MathHelper::Normalize(vertex.Normal);

	m_geometry.texVertices = vertices;//.data();
	m_geometry.VertexBufferSize = static_cast<UINT>(vertices.size()) * sizeof(TexturedVertex);
	m_geometry.VertexByteStride = sizeof(TexturedVertex);

	m_geometry.indices = indicies;
	m_geometry.IndexBufferSize = static_cast<UINT>(m_geometry.indices.size()) * sizeof(uint16_t);

	return true;
}
