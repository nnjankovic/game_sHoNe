#include "Lights.h"

#include <iostream>
#include <sstream>

void OutputDebug(LightTypes::Vector3 vec3, std::wstring desc) {
	std::wstringstream ss;
	ss << desc << " {" << vec3.x << L", " << vec3.y << L", " << vec3.z << L"}" << std::endl;
	OutputDebugString(ss.str().c_str());
}

void LightTypes::Light::Up()
{
	if (m_params.lightType == LightTypes::LightType::DIRECTIONAL)
		return;

	float inc = 1;
	m_params.Position.y += inc;
	m_dirty = true;

	OutputDebug(m_params.Position, L"Posiiton: ");
}

void LightTypes::Light::Down()
{
	if (m_params.lightType == LightTypes::LightType::DIRECTIONAL)
		return;

	float inc = 1;
	m_params.Position.y -= inc;
	m_dirty = true;

	OutputDebug(m_params.Position, L"Posiiton: ");
}

void LightTypes::Light::Left()
{
	if (m_params.lightType == LightTypes::LightType::DIRECTIONAL)
		return;

	float inc = 1;
	m_params.Position.x -= inc;
	m_dirty = true;

	OutputDebug(m_params.Position, L"Posiiton: ");
}

void LightTypes::Light::Right()
{
	if (m_params.lightType == LightTypes::LightType::DIRECTIONAL)
		return;

	float inc = 1;
	m_params.Position.x += inc;
	m_dirty = true;

	OutputDebug(m_params.Position, L"Posiiton: ");
}

void LightTypes::Light::Forward()
{
	if (m_params.lightType == LightTypes::LightType::DIRECTIONAL)
		return;

	float inc = 1;
	m_params.Position.z += inc;
	m_dirty = true;

	OutputDebug(m_params.Position, L"Posiiton: ");
}

void LightTypes::Light::Back()
{
	if (m_params.lightType == LightTypes::LightType::DIRECTIONAL)
		return;

	float inc = 1;
	m_params.Position.z -= inc;
	m_dirty = true;

	OutputDebug(m_params.Position, L"Posiiton: ");
}

void LightTypes::Light::IncreaseStrength()
{
	float inc = 0.05f;
	if (m_params.Strength.x + inc > 1.0f || m_params.Strength.y + inc > 1.0f || m_params.Strength.z + inc > 1.0f)
		return;

	m_params.Strength.x += inc;
	m_params.Strength.y += inc;
	m_params.Strength.z += inc;

	m_dirty = true;

	OutputDebug(m_params.Strength, L"Strength: ");
}

void LightTypes::Light::DecreaseStrength()
{
	float dec = 0.05f;
	if (m_params.Strength.x - dec < -1.0f || m_params.Strength.y - dec < -1.0f || m_params.Strength.z - dec < -1.0f)
		return;

	m_params.Strength.x -= dec;
	m_params.Strength.y -= dec;
	m_params.Strength.z -= dec;

	m_dirty = true;

	OutputDebug(m_params.Strength, L"Strength: ");
}

void LightTypes::Light::ChangeDirectionX()
{
	if (m_params.lightType == LightTypes::LightType::POINT)
		return;

	float inc = 0.05f;

	m_params.Direction.x += inc;
	if (m_params.Direction.x > 1)
		m_params.Direction.x = -1;

	m_dirty = true;

	OutputDebug(m_params.Direction, L"Direction: ");
}

void LightTypes::Light::ChangeDirectionY()
{
	if (m_params.lightType == LightTypes::LightType::POINT)
		return;

	float inc = 0.05f;

	m_params.Direction.y += inc;
	if (m_params.Direction.y > 1)
		m_params.Direction.y = -1;

	m_dirty = true;

	OutputDebug(m_params.Direction, L"Direction: ");
}

void LightTypes::Light::ChangeDirectionZ()
{
	if (m_params.lightType == LightTypes::LightType::POINT)
		return;

	float inc = 0.05f;

	m_params.Direction.z += inc;
	if (m_params.Direction.z > 1)
		m_params.Direction.z = -1;

	m_dirty = true;

	OutputDebug(m_params.Direction, L"Direction: ");
}

void LightTypes::Light::OnUserButtonPressed(std::wstring button)
{
	if (button == L"UP")
	{
		Up();
	}
	else if (button == L"DOWN")
	{
		Down();
	}
	else if (button == L"LEFT")
	{
		Left();
	}
	else if (button == L"RIGHT")
	{
		Right();
	}
	else if (button == L"FORWARD")
	{
		Forward();
	}
	else if (button == L"BACK")
	{
		Back();
	}
	else if (button == L"PLUS")
	{
		IncreaseStrength();
	}
	else if (button == L"MINUS")
	{
		DecreaseStrength();
	}
	else if (button == L"B")
	{
		ChangeDirectionX();
	}
	else if (button == L"N")
	{
		ChangeDirectionY();
	}
	else if (button == L"M")
	{
		ChangeDirectionZ();
	}
}
