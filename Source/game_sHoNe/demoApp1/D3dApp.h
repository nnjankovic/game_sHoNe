#pragma once
#ifndef D3DAPP_H_
#define D3DAPP_H_

#include"Utils.h"
#include"GameTimer.h"

class D3dApp
{
protected:
	D3dApp(HINSTANCE hInstance) {};
	D3dApp(const D3dApp& rhs) = delete;
	D3dApp& operator= (const D3dApp& rhs) = delete;
	virtual ~D3dApp() {};
public:
	static std::shared_ptr<D3dApp> getInstance() { return nullptr; }
	
	HINSTANCE AppInst() const;
	HWND MainWnd() const;
	float AspectRatio() const;

};

#endif

