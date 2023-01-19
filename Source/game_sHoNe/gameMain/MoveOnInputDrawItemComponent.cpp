#include "MoveOnInputDrawItemComponent.h"

Renderer3D::MoveOnInputDrawItemComponent::MoveOnInputDrawItemComponent(std::shared_ptr<DrawItem> drawItemPtr, 
																	   std::function<void(DrawItemProperties&, std::wstring button)> f, 
																	   std::shared_ptr<UserControllerListener> userControlListener) :
	m_DrawItem(drawItemPtr),
	m_function(f),
	m_userControlListener(userControlListener)
{
}

void Renderer3D::MoveOnInputDrawItemComponent::Init()
{
	m_userControlListener->RegisterCallback(this);
}

void Renderer3D::MoveOnInputDrawItemComponent::Exit()
{
	m_userControlListener->UnRegisterCallback(this);
}
