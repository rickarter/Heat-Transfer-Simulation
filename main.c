#include <windows.h>

#define WINDOW_CLASS_NAME "ThermalConductivity"

LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nShowCmd)
{
	HWND hWnd;

	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(wc));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = TEXT(WINDOW_CLASS_NAME);

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(
		0,
		TEXT(WINDOW_CLASS_NAME),
		TEXT(WINDOW_CLASS_NAME),
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		500,
		500,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	ShowWindow(hWnd, SW_SHOW);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
			break;
	}

	return DefWindowProc (hWnd, message, wParam, lParam);
}
