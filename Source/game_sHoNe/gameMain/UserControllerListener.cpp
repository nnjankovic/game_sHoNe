#include "UserControllerListener.h"
#include "WindowConstants.h"

#include <iostream>

LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	UserControllerListener* userController = NULL;

	if (message == WM_NCCREATE) {
		CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
		userController = (UserControllerListener*)cs->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)userController);
	}
	else {
		userController = (UserControllerListener*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (userController)
			return userController->windowProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

UserControllerListener::UserControllerListener(HINSTANCE hInst) : m_hInstance(hInst) 
{
}


UserControllerListener::~UserControllerListener()
{
SendMessage(m_window, WM_CLOSE, 0, 0);
	m_Thread.join();
}

void UserControllerListener::Start()
{
	m_Thread = std::thread([&]() {
		if (!registerWindow()) {
			MessageBox(NULL, L"Call to RegisterClassEx failed!", L"LISTENER WINDOW", NULL);
			return;
		}

		m_window = CreateWindow(
			USER_INPUT_LISTENER.c_str(),
			USER_INPUT_LISTENER.c_str(),
			WS_OVERLAPPEDWINDOW,//WS_CHILD,
			100, 100,
			50, 50,
			0,//m_parentWindow,
			NULL,
			m_hInstance,
			this
		);

		if (!m_window) {
			MessageBox(NULL, L"Call to CreateWindow failed!", L"LISTENER WINDOW", NULL);
			return;
		}

		//ShowWindow(m_window, SW_SHOW);
		//UpdateWindow(m_window);
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
	});
}

LRESULT UserControllerListener::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_APP_KEYDOWN:
	{
		std::wstring button;
		switch (wParam) {
		case 'W':
			button = L"W";
			break;
		case 'S':
			button = L"S";
			break;
		case 'A':
			button = L"A";
			break;
		case 'D':
			button = L"D";
			break;
		case 'Q':
			button = L"Q";
			break;
		case 'E':
			button = L"E";
			break;
		case 'B':
			button = L"B";
			break;
		case 'N':
			button = L"N";
			break;
		case 'M':
			button = L"M";
			break;
		case VK_LEFT:
			button = L"LEFT";
			break;
		case VK_UP:
			button = L"FORWARD";
			break;
		case VK_RIGHT:
			button = L"RIGHT";
			break;
		case VK_DOWN:
			button = L"BACK";
			break;
		case VK_OEM_PLUS:
			button = L"PLUS";
			break;
		case VK_OEM_MINUS:
			button = L"MINUS";
			break;
		case 33 : //PAGE_UP
			button = L"UP";
			break;
		case 34: //PAGE_DOWN
			button = L"DOWN";
			break;
		default:
			std::cout << wParam << " ";
			break;
		}
		fireCallbacks(button);
	}

	break;

	case WM_APP_KEYUP:
	{
		std::wstring button;
		switch (wParam) {
		case 'W':
			button = L"W";
			break;
		case 'S':
			button = L"S";
			break;
		case 'A':
			button = L"A";
			break;
		case 'D':
			button = L"D";
			break;
		case 'Q':
			button = L"Q";
			break;
		case 'E':
			button = L"E";
			break;
		case 'B':
			button = L"B";
			break;
		case 'N':
			button = L"N";
			break;
		case 'M':
			button = L"M";
			break;
		case VK_LEFT:
			button = L"LEFT";
			break;
		case VK_UP:
			button = L"FORWARD";
			break;
		case VK_RIGHT:
			button = L"RIGHT";
			break;
		case VK_DOWN:
			button = L"BACK";
			break;
		case VK_OEM_PLUS:
			button = L"PLUS";
			break;
		case VK_OEM_MINUS:
			button = L"MINUS";
			break;
		case 33: //PAGE_UP
			button = L"UP";
			break;
		case 34: // PAGE_DOWN
			button = L"DOWN";
			break;
		default:
			std::cout << wParam << " ";
			break;
		}
		fireCallbacks(button);

	}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void UserControllerListener::RegisterCallback(IUserControlCallback* callback)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_callbacks.push_back(callback);
}

void UserControllerListener::UnRegisterCallback(IUserControlCallback* callback)
{
	//TO_DO: fix this
	/*std::lock_guard<std::mutex> lock(m_mutex);

	auto iter = m_callbacks.begin();
	while (iter != m_callbacks.end())
	{
		if ((*iter) == callback)
			iter = m_callbacks.erase(iter);
		iter++;
	}*/
}

bool UserControllerListener::registerWindow()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &WndProc1;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(UserControllerListener*);
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = USER_INPUT_LISTENER.c_str();
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (RegisterClassEx(&wcex))
		return true;
	return false;

}

void UserControllerListener::fireCallbacks(std::wstring button)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for (auto& cb : m_callbacks)
	{
			cb->OnUserButtonPressed(button);
	}
}
