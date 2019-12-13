#pragma once

#include "Utility.h"
#include "UserControllerListener.h"
#include "GameTimer.h"

class CameraNode : public IUserControlCallback
{
public:
	CameraNode(std::shared_ptr<UserControllerListener> userControlListener, std::shared_ptr<GameTimer> timer);
	~CameraNode();

	DirectX::XMFLOAT4X4 getViewMatrix();
private:
	// Inherited via IUserControlCallback
	virtual void OnUserButtonPressed(std::wstring button) override;

	void Walk(double d);
	void Strafe(double d);
	void RotateH(double angle);
	void RotateV(double angle);
private:
	DirectX::XMFLOAT3 m_Position = { 0.0f, 2.0f, -15.0f };
	DirectX::XMFLOAT3 m_Right = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_Up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 m_LookAt = { 0.0f, 0.0f, 0.1f };

	DirectX::XMFLOAT4X4 m_viewMatrix;

	std::shared_ptr<GameTimer> m_timer;
	std::shared_ptr<UserControllerListener> m_userControlListener;

	std::mutex m_mutex;

	bool m_ViewDirty = false;
};

