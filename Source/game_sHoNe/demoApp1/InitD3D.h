#pragma once
#include "D3dApp.h"


class InitD3D : public D3dApp
{
public:
	InitD3D(HINSTANCE hInstance);
	~InitD3D();

	virtual bool Initialize() override;
private:
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
};

