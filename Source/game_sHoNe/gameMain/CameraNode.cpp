#include "CameraNode.h"

using namespace DirectX;

CameraNode::CameraNode(std::shared_ptr<UserControllerListener> userControlListener, std::shared_ptr<GameTimer> timer) :
	m_timer(timer),
	m_userControlListener(userControlListener)
{
	m_userControlListener->RegisterCallback(this);
}


CameraNode::~CameraNode()
{
}

DirectX::XMFLOAT4X4 CameraNode::getViewMatrix()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_ViewDirty)
		return m_viewMatrix;

	/*XMVECTOR L = XMLoadFloat3(&m_LookAt);
	L = XMVector3Normalize(L);
	XMStoreFloat3(&m_LookAt, L);

	XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&m_Position), L, XMLoadFloat3(&m_Up));
	XMStoreFloat4x4(&m_viewMatrix, view);*/


	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_LookAt);
	XMVECTOR P = XMLoadFloat3(&m_Position);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_LookAt, L);

	m_viewMatrix(0, 0) = m_Right.x;
	m_viewMatrix(1, 0) = m_Right.y;
	m_viewMatrix(2, 0) = m_Right.z;
	m_viewMatrix(3, 0) = x;

	m_viewMatrix(0, 1) = m_Up.x;
	m_viewMatrix(1, 1) = m_Up.y;
	m_viewMatrix(2, 1) = m_Up.z;
	m_viewMatrix(3, 1) = y;

	m_viewMatrix(0, 2) = m_LookAt.x;
	m_viewMatrix(1, 2) = m_LookAt.y;
	m_viewMatrix(2, 2) = m_LookAt.z;
	m_viewMatrix(3, 2) = z;

	m_viewMatrix(0, 3) = 0.0f;
	m_viewMatrix(1, 3) = 0.0f;
	m_viewMatrix(2, 3) = 0.0f;
	m_viewMatrix(3, 3) = 1.0f;

	m_ViewDirty = false;

	return m_viewMatrix;
}

void CameraNode::OnUserButtonPressed(std::wstring button)
{
	if (button == L"W")
	{
		Walk(555 * m_timer->DeltaTime());
	}
	else if (button == L"S")
	{
		Walk(-555 * m_timer->DeltaTime());
	}
	else if (button == L"A")
	{
		Strafe(-555 * m_timer->DeltaTime());
	}
	else if (button == L"D")
	{
		Strafe(555 * m_timer->DeltaTime());
	}
	else if (button == L"Q")
	{
		RotateH(-77 * m_timer->DeltaTime());
	}
	else if (button == L"E")
	{
		RotateH(77 * m_timer->DeltaTime());
	}

}

void CameraNode::Walk(double d)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	//m_Position += d*m_Look;
	XMVECTOR forward = XMVectorReplicate(d);
	XMVECTOR lookAt = XMLoadFloat3(&m_LookAt);
	XMVECTOR position = XMLoadFloat3(&m_Position);

	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(forward, lookAt, position));

	m_ViewDirty = true;
}

void CameraNode::Strafe(double d)
{

	std::lock_guard<std::mutex> lock(m_mutex);

	//m_Position += d*m_Right;
	XMVECTOR forward = XMVectorReplicate(d);
	XMVECTOR right = XMLoadFloat3(&m_Right);
	XMVECTOR position = XMLoadFloat3(&m_Position);

	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(forward, right, position));

	m_ViewDirty = true;
}

void CameraNode::RotateH(double angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	//XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_LookAt, XMVector3TransformNormal(XMLoadFloat3(&m_LookAt), R));

	m_ViewDirty = true;
}

void CameraNode::RotateV(double angle)
{
}


