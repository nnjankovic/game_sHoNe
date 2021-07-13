#include "DrawItem.h"

unsigned int DrawItem::s_constantBuffer = 0;

void DrawItem::Draw(XMMATRIX matrixStack)
{
	XMMATRIX worldMatrix = XMMatrixTranslation(m_properties.position.x, m_properties.position.y, m_properties.position.z) * XMMatrixRotationX(m_properties.xAngle);
	//worldMatrix *= matrixStack;

	XMStoreFloat4x4(&m_properties.objectConstants.WorldMatrix, worldMatrix);


	m_renderer->Draw(*this);
}
