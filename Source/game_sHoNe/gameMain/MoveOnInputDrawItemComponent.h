#pragma once
#if !defined MOVE_ON_INPUT_DRAW_ITEM_COMPONENT_H_
#define MOVE_ON_INPUT_DRAW_ITEM_COMPONENT_H_

#include "IDrawItemComponent.h"
#include "DrawItem.h"

namespace Renderer3D {

	class MoveOnInputDrawItemComponent : public IDrawItemComponent,
										 public IUserControlCallback {
	public:
		MoveOnInputDrawItemComponent(std::shared_ptr<DrawItem> drawItemPtr, std::function<void(DrawItemProperties& , std::wstring button)> f, std::shared_ptr<UserControllerListener> userControlListener);
		~MoveOnInputDrawItemComponent() {};

		//IDrawItemComponent
		void Init() override;
		void Exit() override;
		void Update() override {};

		//IUserControlCallback
		void OnUserButtonPressed(std::wstring button) override { m_function(m_DrawItem->m_properties, button); };

	private:
		std::shared_ptr<DrawItem> m_DrawItem;
		std::function<void(DrawItemProperties& , std::wstring button)> m_function;
		std::shared_ptr<UserControllerListener> m_userControlListener;
	};

};

#endif
