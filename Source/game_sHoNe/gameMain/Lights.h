#pragma once
#if !defined LIGHTS_H_
#define		 LIGHTS_H_

#include "UserControllerListener.h"

namespace LightTypes {

	enum class LightType {
		DIRECTIONAL,
		POINT,
		SPOT
	};

	struct Vector3 {
		float x;
		float y;
		float z;
	};

	struct LightParams {
		Vector3 Strength = { 0.5f, 0.5f, 0.5f };
		float FalloffStart = 1.0f;                          // point/spot light only
		Vector3 Direction = { 0.0f, -1.0f, 0.0f };			// directional/spot light only
		float FalloffEnd = 10.0f;                           // point/spot light only
		Vector3 Position = { 0.0f, 0.0f, 0.0f };			// point/spot light only
		float SpotPower = 64.0f;                            // spot light only
		LightType lightType = LightType::DIRECTIONAL;
	};

	class Light : public IUserControlCallback {
	public:
		Light() {}
		Light(std::shared_ptr<UserControllerListener> userInputListener, LightParams lightParams) :
			m_userInputListenerPtr(userInputListener),
			m_params(lightParams)
		{
			if(nullptr != m_userInputListenerPtr)
				m_userInputListenerPtr->RegisterCallback(this);
		}
		~Light(){
			if (nullptr != m_userInputListenerPtr)
				m_userInputListenerPtr->UnRegisterCallback(this);
		}
		
		bool isDirty() { return m_dirty; };
		void ClearDirtyFlag() { m_dirty = false; }

		LightParams getLightParams() { return m_params; }
	private:
		std::shared_ptr<UserControllerListener> m_userInputListenerPtr = nullptr;
		bool m_dirty = true;
		LightParams m_params;

	private:
		void Up();
		void Down();
		void Left();
		void Right();
		void Forward();
		void Back();

		void IncreaseStrength();
		void DecreaseStrength();

		void ChangeDirectionX();
		void ChangeDirectionY();
		void ChangeDirectionZ();

		// Inherited via IUserControlCallback
		void OnUserButtonPressed(std::wstring button) override;
		
	};
}

#endif
