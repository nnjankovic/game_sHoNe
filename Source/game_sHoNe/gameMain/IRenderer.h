#pragma once
#ifndef IRENDERER_H_
#define IRENDERER_H_

class DrawItem;

struct IRenderer {
	virtual bool Init() = 0;
	virtual bool Exit() = 0;

	virtual bool PrepareDraw() = 0;
	virtual bool Draw(const DrawItem& drawItem) = 0;
	virtual bool UploadStaticGeometry(DrawItem& drawItem) = 0;
	virtual void Present() = 0;
	virtual void createPSO(DrawItem& drawItem) = 0;

	virtual ~IRenderer() {};
};


#endif // !IRENDERER_H_

