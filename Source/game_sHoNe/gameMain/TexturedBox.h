#pragma once
#include "DrawItem.h"
class TexturedBox :
	public Renderer3D::DrawItem
{
public:
	TexturedBox(std::shared_ptr<Renderer3D::IRenderer> renderer, float x, float y, float z, float width, float height, float depth, 
		const Renderer3D::Texture& tex,
		const Renderer3D::Material& mat);
	~TexturedBox();

	// Inherited via DrawItem
	//virtual bool createShadersAndInputLayout() override;
	virtual bool loadGeometry() override;

private:
	float m_width;
	float m_height;
	float m_depth;
};

