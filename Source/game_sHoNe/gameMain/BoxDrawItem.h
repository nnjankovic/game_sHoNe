#pragma once

#include "DrawItem.h"

class BoxDrawItem : public DrawItem
{
public:
	BoxDrawItem(std::shared_ptr<IRenderer> renderer, float x, float y, float z, float width = 1, float height = 1, float depth = 1);
	~BoxDrawItem();

private:
	// Inherited via DrawItem
	//bool createShadersAndInputLayout() override;
	bool loadGeometry() override;

private:
	float m_width;
	float m_height;
	float m_depth;
};

