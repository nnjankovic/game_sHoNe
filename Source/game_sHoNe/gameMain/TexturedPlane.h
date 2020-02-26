#pragma once
#include "DrawItem.h"



class TexturedPlane :
	public DrawItem
{
public:
	TexturedPlane(std::shared_ptr<IRenderer> renderer, MathHelper::PositionVector position, float angle, Texture tex);

	// Inherited via DrawItem
	virtual bool createShadersAndInputLayout() override;
	virtual bool loadGeometry() override;
};

