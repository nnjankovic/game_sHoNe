#pragma once
#include "DrawItem.h"
#include "Utility.h"

class PlaneDrawItem :
	public Renderer3D::DrawItem
{
public:
	PlaneDrawItem(std::shared_ptr<Renderer3D::IRenderer> renderer, MathHelper::Position3 position, float angle);
	~PlaneDrawItem();

private:
	// Inherited via DrawItem
	//bool createShadersAndInputLayout() override;
	bool loadGeometry() override;

private:
	float m_angle;
};

