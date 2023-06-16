//Draw Items loaded from .obj files
#pragma once
#if !defined LOADED_DRAW_ITEM_H_
#define		 LOADED_DRAW_ITEM_H_

#include "DrawItem.h"

class LoadedDrawItem : public Renderer3D::DrawItem {
public:
	LoadedDrawItem(std::shared_ptr<Renderer3D::IRenderer> renderer, std::wstring path, MathHelper::Position3 position, float angle, float scaleFactor,
		const Renderer3D::Texture& tex, const Renderer3D::Material& mat, Renderer3D::ShaderType shaderType = Renderer3D::ShaderType::Textured);
	~LoadedDrawItem() {};
private:
	//DrawItem
	bool loadGeometry() override;
	bool computeNormals() override { return true; };

private:
	std::wstring m_modelPath;

};


#endif