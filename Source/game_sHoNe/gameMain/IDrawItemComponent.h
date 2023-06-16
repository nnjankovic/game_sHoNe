#pragma once
#if !defined IDRAW_ITEM_COMPONENT_H_
#define IDRAW_ITEM_COMPONENT_H_

#include <functional>
#include <memory>

namespace Renderer3D {

struct IDrawItemComponent {
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Exit() = 0;
};

}
#endif