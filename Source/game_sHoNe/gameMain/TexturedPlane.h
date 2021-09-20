#pragma once
#include "DrawItem.h"



class TexturedPlane :
	public Renderer3D::DrawItem
{
public:
	TexturedPlane(std::shared_ptr<Renderer3D::IRenderer> renderer, MathHelper::PositionVector position, float angle, 
		const Renderer3D::Texture& tex, const Renderer3D::Material& mat);

	// Inherited via DrawItem
	//virtual bool createShadersAndInputLayout() override;
	virtual bool loadGeometry() override;
};

