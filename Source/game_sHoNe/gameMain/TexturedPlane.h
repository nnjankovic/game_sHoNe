#pragma once
#include "DrawItem.h"



class TexturedPlane :
	public Renderer3D::DrawItem
{
public:
	TexturedPlane(std::shared_ptr<Renderer3D::IRenderer> renderer, MathHelper::Position3 position, 
		int sizeM, int sizeN, float angle, int scaling, 
		const Renderer3D::Texture& tex, MathHelper::Position2 textureScale,
		const Renderer3D::Material& mat, Renderer3D::ShaderType shaderType = Renderer3D::ShaderType::Textured);

	~TexturedPlane();

	// Inherited via DrawItem
	//virtual bool createShadersAndInputLayout() override;
	virtual bool loadGeometry() override;
private:
	int m_m;
	int m_n;
};

