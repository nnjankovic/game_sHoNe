#pragma once
#include "DrawItem.h"
class TexturedBox :
	public DrawItem
{
public:
	TexturedBox(std::shared_ptr<IRenderer> renderer, float x, float y, float z, float width, float height, float depth, Texture tex);
	~TexturedBox();

	// Inherited via DrawItem
	//virtual bool createShadersAndInputLayout() override;
	virtual bool loadGeometry() override;

private:
	float m_width;
	float m_height;
	float m_depth;
};

