#pragma once
#include <string>
#include "Utility.h"

#include <thread>
#include <mutex>
#include <list>

struct IUserControlCallback {
	virtual void OnUserButtonPressed(std::wstring button) = 0;

	virtual ~IUserControlCallback() {};
};

class UserControllerListener
{
public:
	UserControllerListener(HINSTANCE hInst);
	~UserControllerListener();

	void Start();

	LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void RegisterCallback(IUserControlCallback* callback);
	void UnRegisterCallback(IUserControlCallback* callback);
private:
	bool registerWindow();

	void fireCallbacks(std::wstring button);
private:

	HWND m_window;
	HWND m_parentWindow;
	HINSTANCE m_hInstance;

	std::vector<IUserControlCallback*> m_callbacks;
private:
	std::thread m_Thread;
	std::mutex m_mutex;
};

