#include "DrawItem.h"

void DrawItem::Draw(XMMATRIX matrixStack)
{
	XMMATRIX worldMatrix = XMMatrixTranslation(m_properties.position.x, m_properties.position.y, m_properties.position.z);
	worldMatrix *= matrixStack;

	XMStoreFloat4x4(&m_properties.objectConstants.WorldViewProj, worldMatrix);


	m_renderer->Draw(*this);
}
