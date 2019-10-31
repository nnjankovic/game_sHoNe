#pragma once

#include "DrawItem.h"

class BoxDrawItem : public DrawItem
{
public:
	BoxDrawItem(std::shared_ptr<IRenderer> renderer);
	~BoxDrawItem();

private:
	// Inherited via DrawItem
	bool createShadersAndInputLayout() override;
	bool loadGeometry() override;
};

