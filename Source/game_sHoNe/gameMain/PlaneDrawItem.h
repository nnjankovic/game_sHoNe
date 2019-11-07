#pragma once
#include "DrawItem.h"
class PlaneDrawItem :
	public DrawItem
{
public:
	PlaneDrawItem(std::shared_ptr<IRenderer> renderer);
	~PlaneDrawItem();

private:
	// Inherited via DrawItem
	bool createShadersAndInputLayout() override;
	bool loadGeometry() override;
};

