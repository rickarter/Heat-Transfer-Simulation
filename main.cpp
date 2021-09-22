#include <windows.h>
#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

#define WINDOW_CLASS_NAME "ThermalConductivity"
#define SCREEN_WIDTH 800
#define SCRENN_HEIGHT 800

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;
ID3D11RenderTargetView *backbuffer;

void InitD3D(HWND hWnd);
void CleanD3D(void);
void RenderFrame(void);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
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
	
	RECT wr = {0, 0, SCREEN_WIDTH, SCRENN_HEIGHT};
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(
		0,
		TEXT(WINDOW_CLASS_NAME),
		TEXT(WINDOW_CLASS_NAME),
		WS_OVERLAPPEDWINDOW,
		500,
		100,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(hWnd, SW_SHOW);

	InitD3D(hWnd);
	BOOL run = TRUE;
	while (run)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				run = FALSE;
		}

		RenderFrame();
	}

	CleanD3D();
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

void InitD3D(HWND hWnd)
{
	/*	Direct3D initialization	*/
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCRENN_HEIGHT;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon
	);

	/*	Set the render target	*/
	// Get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// Use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();
	
	// Set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL); 

	/*	Set the viewport	*/
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCRENN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);
}

void CleanD3D()
{
	swapchain->SetFullscreenState(FALSE, NULL);

	// Close and release all existing COM objects
	swapchain->Release();
	dev->Release();
	devcon->Release();
}

void RenderFrame()
{
	// Clear the backbuffer
	float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, clearColor);


	swapchain->Present(0, 0);
}
