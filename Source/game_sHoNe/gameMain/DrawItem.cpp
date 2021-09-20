#include "DrawItem.h"

namespace Renderer3D {
	unsigned int DrawItem::s_constantBuffer = 0;

	void DrawItem::Draw(XMMATRIX matrixStack)
	{
		XMMATRIX worldMatrix = XMMatrixTranslation(m_properties.position.x, m_properties.position.y, m_properties.position.z) * XMMatrixRotationX(m_properties.xAngle);
		//worldMatrix *= matrixStack;

		XMStoreFloat4x4(&m_properties.objectConstants.WorldMatrix, XMMatrixTranspose(worldMatrix));


		m_renderer->Draw(*this);
	}

	bool DrawItem::computeNormals()
	{
		if (m_properties.shaderType == ShaderType::PLAIN)
			return false;

		for (int i = 0; i < m_geometry.indices.size() - 3; i += 3)
		{
			unsigned int p0 = m_geometry.indices[i];
			unsigned int p1 = m_geometry.indices[i + 1];
			unsigned int p2 = m_geometry.indices[i + 2];

			XMVECTOR u = XMLoadFloat3(&m_geometry.texVertices[p1].Pos) - XMLoadFloat3(&m_geometry.texVertices[p0].Pos);
			XMVECTOR v = XMLoadFloat3(&m_geometry.texVertices[p2].Pos) - XMLoadFloat3(&m_geometry.texVertices[p0].Pos);
			XMVECTOR normal = XMVector3Normalize(XMVector3Cross(u, v));

			XMStoreFloat3(&m_geometry.texVertices[m_geometry.indices[i]].Normal, normal);
			XMStoreFloat3(&m_geometry.texVertices[m_geometry.indices[i + 1]].Normal, normal);
			XMStoreFloat3(&m_geometry.texVertices[m_geometry.indices[i + 2]].Normal, normal);
		}

		return true;
	}
};