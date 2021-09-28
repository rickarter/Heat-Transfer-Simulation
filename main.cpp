#include <windows.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <DirectXmath.h>
#include <stdio.h>

using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3Dcompiler.lib")

#define WINDOW_CLASS_NAME "ThermalConductivity"
#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;
ID3D11RenderTargetView *backbuffer;
ID3D11InputLayout *pLayout;

ID3D11UnorderedAccessView *pUAView;
ID3D11ShaderResourceView *pSRView;

ID3D11VertexShader *pVS;
ID3D11PixelShader *pPS;
ID3D11ComputeShader *pCS;

ID3D11Buffer *pVertexBuffer;
ID3D11Buffer *pIndexBuffer;
ID3D11Buffer *pComputeBuffer;
ID3D11Buffer *pComputeResultBuffer;

struct Vertex
{
	XMFLOAT4 position;
};

struct ComputeData 
{
	float energy;
};

void InitD3D(HWND hWnd);
void CleanD3D(void);
void RenderFrame(void);
void InitPipeline(void);
void InitGraphics(void);

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
	
	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(
		0,
		TEXT(WINDOW_CLASS_NAME),
		TEXT(WINDOW_CLASS_NAME),
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU),
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
	InitPipeline();
	InitGraphics();

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

		if (run)
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
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return 0;
			}
			break;
	}

	return DefWindowProc (hWnd, message, wParam, lParam);
}

void RenderFrame()
{
	// Clear the backbuffer
	float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, clearColor);

	devcon->Dispatch(1, 500, 1);

	devcon->CopyResource(pComputeResultBuffer, pComputeBuffer);

	// Read data from Compute Shader
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	devcon->Map(pComputeResultBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

	ComputeData *p;
	p = (ComputeData*)mappedResource.pData;

	float t = p[2].energy;
	char buffer[256] = {};
	sprintf(buffer, "%f", t);
	MessageBox(NULL, buffer, "Debug", NULL);

	devcon->Unmap(pComputeResultBuffer, 0);

	devcon->DrawIndexed(6, 0, 0);

	swapchain->Present(0, 0);
}

void InitD3D(HWND hWnd)
{
	/*	Direct3D initialization	*/
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
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
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);
}

void InitGraphics()
{
	Vertex vertices [] = 
	{
		XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f),
		XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f)
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.ByteWidth = sizeof(Vertex) * 4;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	
	InitData.pSysMem = vertices;
	dev->CreateBuffer(&bd, &InitData, &pVertexBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// Index buffer
	unsigned int indices[] = { 0, 1, 2, 3, 0, 2 };

	ZeroMemory(&bd, sizeof(bd)); // Clean bd (Decleared above)
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData)); // Clean InitData (Decleared above)
	InitData.pSysMem = indices;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&bd, &InitData, &pIndexBuffer);

	devcon->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Compute shader buffer
	/*ComputeData data[] = 
	{
		{ XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT4(0.0f, 0.6f, 0.0f, 1.0f) }
	};*/
	
	/*const unsigned int dataSize = 2;
	ComputeData data[dataSize];
	ComputeData initData;
	initData.color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	for(int i = 0; i < dataSize; i++)
	{
		data[i] = initData;
	}*/

	//ComputeData data[SCREEN_WIDTH * SCREEN_HEIGHT];
	//ZeroMemory(&data, sizeof(data));
	//
	const size_t dataSize = SCREEN_WIDTH * SCREEN_HEIGHT;
	ComputeData data[dataSize];
	for(int i = 0; i < dataSize; i++)
	{
		data[i].energy = 0.0f;
	}
	data[0].energy = 100.0f;

	ZeroMemory(&bd, sizeof(bd)); // Clean bd (Decleared above)
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ComputeData) * dataSize;
	bd.StructureByteStride = sizeof(ComputeData);
	bd.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData)); // Clean InitData (Decleared above)
	InitData.pSysMem = data;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	dev->CreateBuffer(&bd, &InitData, &pComputeBuffer);

	// Set UAV for compute shader
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
	ZeroMemory(&uavd, sizeof(uavd));

	uavd.Buffer.FirstElement = 0;
	uavd.Buffer.Flags = 0;
	uavd.Buffer.NumElements = dataSize;
	uavd.Format = DXGI_FORMAT_UNKNOWN;
	uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	dev->CreateUnorderedAccessView(pComputeBuffer, &uavd, &pUAView);

	devcon->CSSetUnorderedAccessViews(0, 1, &pUAView, NULL);

	// Compute result buffer
	bd.Usage = D3D11_USAGE_STAGING;
	bd.BindFlags = 0;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	dev->CreateBuffer(&bd, 0, &pComputeResultBuffer);

	// Set SRV for Pixel Shader
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));

	srvd.BufferEx.FirstElement = 0;
	srvd.BufferEx.Flags = 0;
	srvd.BufferEx.NumElements = dataSize; 
	srvd.Format = DXGI_FORMAT_UNKNOWN;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;

	HRESULT hr = dev->CreateShaderResourceView(pComputeBuffer, &srvd, &pSRView);
	assert(SUCCEEDED(hr));

	devcon->PSSetShaderResources(0, 1, &pSRView);

	// Set topology
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void InitPipeline()
{
	ID3DBlob *vsBlob, *psBlob, *csBlob;
	D3DCompileFromFile(L"shaders.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0", 0, 0, &vsBlob, NULL); 
	D3DCompileFromFile(L"shaders.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", 0, 0, &psBlob, NULL);
	D3DCompileFromFile(L"compute.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CSMain", "cs_5_0", 0, 0, &csBlob, NULL);

	dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pPS);
	dev->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, &pCS);

	devcon->VSSetShader(pVS, NULL, NULL);
	devcon->PSSetShader(pPS, NULL, NULL);
	devcon->CSSetShader(pCS, NULL, NULL);

    D3D11_INPUT_ELEMENT_DESC ied[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    dev->CreateInputLayout(ied, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);
}

void CleanD3D()
{
	swapchain->SetFullscreenState(FALSE, NULL);

	// Close and release all existing COM objects
	pVS->Release();
	pPS->Release();
	pCS->Release();

	pLayout->Release();
	pUAView->Release();
	pSRView->Release();

	pVertexBuffer->Release();
	pIndexBuffer->Release();
	pComputeBuffer->Release();
	pComputeResultBuffer->Release();

	swapchain->Release();
	backbuffer->Release();

	dev->Release();
	devcon->Release();
}
