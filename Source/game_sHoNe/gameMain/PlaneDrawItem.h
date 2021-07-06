#pragma once
#include "DrawItem.h"
#include "Utility.h"

class PlaneDrawItem :
	public DrawItem
{
public:
	PlaneDrawItem(std::shared_ptr<IRenderer> renderer, MathHelper::PositionVector position, float angle);
	~PlaneDrawItem();

private:
	// Inherited via DrawItem
	//bool createShadersAndInputLayout() override;
	bool loadGeometry() override;

private:
	float m_angle;
};

