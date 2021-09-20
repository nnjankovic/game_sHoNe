#pragma once
#ifndef IRENDERER_H_
#define IRENDERER_H_

#include <memory>
#include <vector>
#include <Windows.h>
#include <DirectXMath.h>

#include "Lights.h"

namespace Renderer3D{
	class DrawItem;
	struct Texture;
	struct Material;

	enum class ShaderType {
		PLAIN,
		TEXTURED
	};

	struct IRenderer {
		virtual bool Init() = 0;
		virtual bool Exit() = 0;

		virtual bool PrepareDraw() = 0;
		virtual bool Draw(DrawItem& drawItem) = 0;
		virtual bool UploadStaticGeometry(std::vector<std::shared_ptr<DrawItem>> staticDrawItems) = 0;
		virtual bool UploadTexture(Texture& texture) = 0;
		virtual void Present() = 0;
		//virtual void createPSO(DrawItem& drawItem) = 0;

		virtual void setWindowTitle(std::wstring title) = 0;

		virtual void setCameraView(const DirectX::XMFLOAT4X4& CameraViewMatrix) = 0;
		virtual void setEyePosition(DirectX::XMFLOAT3 eyePosition) = 0;

		virtual bool addLight(LightTypes::LightParams light) = 0;
		virtual void clearLights() = 0;

		virtual ~IRenderer() {};
	};
};

#endif // !IRENDERER_H_

