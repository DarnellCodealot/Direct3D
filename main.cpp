#include <Chrono>
#include <iostream>
#include <fstream>
#include "graphics.h"
#include "window.h"

/*
LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_CLOSE) {
		PostQuitMessage(0);
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}

int main() {
	FreeConsole();
	//descriptor structure for the window class
	WNDCLASSEX* wc = new WNDCLASSEX;
	wc->cbClsExtra = 0;
	wc->cbSize = sizeof(WNDCLASSEX);
	wc->hInstance = GetModuleHandle(NULL);
	wc->lpszClassName = L"wndclass";
	wc->cbWndExtra = 0;
	wc->hbrBackground = nullptr;
	wc->hCursor = nullptr;
	wc->hIcon = nullptr;
	wc->lpszMenuName = nullptr;
	wc->style = CS_OWNDC;
	wc->hIconSm = nullptr;
	wc->lpfnWndProc = WndProc;

	//registering the window class
	RegisterClassEx(wc);

	delete wc;

	//creating a window of that class
	HWND wnd = CreateWindowEx(0, L"wndclass", L"ShittyTriangle", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 250, 250, 640, 480, nullptr, nullptr, GetModuleHandle(NULL), nullptr);
	
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;
	ID3D11RenderTargetView* pRenderTargetView = nullptr;

	//descriptor stucture for creating the device and swapchain
	DXGI_SWAP_CHAIN_DESC* sd = new DXGI_SWAP_CHAIN_DESC;
	sd->BufferDesc.Width = 0;
	sd->BufferDesc.Height = 0;
	sd->BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd->BufferDesc.RefreshRate.Numerator = 0;
	sd->BufferDesc.RefreshRate.Denominator = 0;
	sd->BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd->BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd->SampleDesc.Count = 1;
	sd->SampleDesc.Quality = 0;
	sd->BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd->BufferCount = 1;
	sd->OutputWindow = wnd;
	sd->BufferCount = 1;
	sd->Flags = 0;
	sd->SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd->Windowed = TRUE;

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, sd, &pSwapChain, &pDevice, nullptr, &pDeviceContext);
	
	delete sd;

	//creating the render target view
	ID3D11Resource* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (void**)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
	pBackBuffer->Release();

	pDeviceContext->OMSetRenderTargets(1u, &pRenderTargetView, nullptr);

	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//displaying the window
	ShowWindow(wnd, SW_SHOW);

	struct Vertex
	{
		float x, y;
	};
	
	const Vertex vertices[3] = { {0.0f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f}};
	
	//creating the vertex buffer
	D3D11_BUFFER_DESC *bd = new D3D11_BUFFER_DESC;
	bd->BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd->ByteWidth = sizeof(vertices);
	bd->Usage = D3D11_USAGE_DEFAULT;
	bd->CPUAccessFlags = 0u;
	bd->MiscFlags = 0u;
	bd->StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA *subdata = new D3D11_SUBRESOURCE_DATA;
	subdata->pSysMem = vertices;
	
	ID3D11Buffer* pVertexBuffer = nullptr;

	pDevice->CreateBuffer(bd, subdata, &pVertexBuffer);
	delete bd;
	delete subdata;

	const UINT* stride = new UINT(sizeof(Vertex));
	const UINT* offset = new UINT(0u);
	pDeviceContext->IASetVertexBuffers(0u, 1u, &pVertexBuffer, stride, offset);
	delete stride;
	delete offset;

	//loading the vertex shader
	std::ifstream* vertexShaderFile = new std::ifstream("VertexShader.cso", std::ios::binary);
	vertexShaderFile->seekg(0, std::ios::end);
	const int* vfilesize = new int(vertexShaderFile->tellg());
	vertexShaderFile->seekg(0, std::ios::beg);
	char* vfilebytedata = new char[*vfilesize];
	int i = 0;
	for (char byte = vertexShaderFile->get(); vertexShaderFile->good(); byte = vertexShaderFile->get()) {
		vfilebytedata[i] = byte;
		i++;
	}
	ID3D11VertexShader* vertexShader = nullptr;
	pDevice->CreateVertexShader((void*)vfilebytedata, *vfilesize, nullptr, &vertexShader);
	pDeviceContext->VSSetShader(vertexShader, nullptr, 0u);

	//setting the input layout
	ID3D11InputLayout* inputLayout = nullptr;
	D3D11_INPUT_ELEMENT_DESC* ild = new D3D11_INPUT_ELEMENT_DESC[1]{
		{"Position", 0u, DXGI_FORMAT_R32G32_FLOAT,0u,0u, D3D11_INPUT_PER_VERTEX_DATA, 0u}
	};
	
	pDevice->CreateInputLayout(ild, 1, vfilebytedata, *vfilesize, &inputLayout);

	pDeviceContext->IASetInputLayout(inputLayout);
	

	delete vertexShaderFile;
	delete [] vfilebytedata;
	delete vfilesize;
	
	//loading the pixel shader
	std::ifstream* pixelShaderFile = new std::ifstream("PixelShader.cso", std::ios::binary);
	pixelShaderFile->seekg(0, std::ios::end);
	const int* pfilesize = new int(pixelShaderFile->tellg());
	pixelShaderFile->seekg(0, std::ios::beg);
	char* pfilebytedata = new char[*pfilesize];
	i = 0;
	for (char byte = pixelShaderFile->get(); pixelShaderFile->good(); byte = pixelShaderFile->get()) {
		pfilebytedata[i] = byte;
		i++;
	}
	ID3D11PixelShader* pixelShader = nullptr;
	pDevice->CreatePixelShader((void*)pfilebytedata, *pfilesize, nullptr, &pixelShader);
	pDeviceContext->PSSetShader(pixelShader, nullptr, 0u);
	delete pixelShaderFile;
	delete[] pfilebytedata;
	delete pfilesize;

	//setting the viewport
	D3D11_VIEWPORT* vp = new D3D11_VIEWPORT;
	vp->TopLeftX = 0;
	vp->TopLeftY = 0;
	vp->Width = 640;
	vp->Height = 480;
	vp->MaxDepth = 1;
	vp->MinDepth = 0;

	pDeviceContext->RSSetViewports(1u, vp);
	delete vp;

	MSG msg = { 0 };
	std::chrono::steady_clock::time_point timepoint = std::chrono::steady_clock::now();
	//main loop
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColor);

		float timePassed = std::chrono::duration<float>(std::chrono::steady_clock::now() - timepoint).count();
	
		pDeviceContext->Draw(3u, 0u);
		
		//swap buffers
		pSwapChain->Present(2u, 0u);
	}

	if (pDevice != nullptr) {
		pDevice->Release();
	}
	if (pSwapChain != nullptr) {
		pDevice->Release();
	}
	if (pDeviceContext != nullptr) {
		pDeviceContext->Release();
	}
	if (pRenderTargetView != nullptr){
		pRenderTargetView->Release();
    }

	DestroyWindow(wnd);
	UnregisterClass(L"wndclass", GetModuleHandle(NULL));
	return 0;
}
*/

int main() {
	Window wnd(640, 480, "Direct3D Shenanigans");
	Graphics gfx(wnd.hWnd);
	Mesh Zucca("Suzanne.obj", Vec3<float>(0.0f,0.0f,8.0f));

	
	std::vector<Vec3<float>> vertices =
	{
		{ -1.0f,-1.0f,-1.0f	 },
		{ 1.0f,-1.0f,-1.0f	 },
		{ -1.0f,1.0f,-1.0f	 },
		{ 1.0f,1.0f,-1.0f	  },
		{ -1.0f,-1.0f,1.0f	 },
		{ 1.0f,-1.0f,1.0f	  },
		{ -1.0f,1.0f,1.0f	 },
		{ 1.0f,1.0f,1.0f	 },
	};
	std::vector<int> indices =
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	Mesh Cube(vertices, indices, Vec3<float>(0,0,2));
	
	std::chrono::steady_clock::time_point timepoint = std::chrono::steady_clock::now();
	
	//main loop
	while (wnd.Run()) {
		
		float timePassed = std::chrono::duration<float>(std::chrono::steady_clock::now() - timepoint).count();

		if (GetAsyncKeyState('A')) {
			Cube.origin.z += 0.05;
		}
		if (GetAsyncKeyState('S')) {
			Cube.origin.z -= 0.05;
		}

		struct{float time;float zdis;}data1{timePassed, Zucca.origin.z};
		
		gfx.ClearBuffer(0, 0, 0);
		gfx.SetConstantBuffer((void*)&data1, 1);//update the constant buffer that the vertex shader uses for Zucca's vertices
		gfx.DrawMesh(Zucca, 255,0,0);
		gfx.SetConstantBuffer((void*)&Cube.origin.z, 0);
		//gfx.DrawMesh(Cube, 0,0,255);
		gfx.EndFrame();
	}

	return 0;
}

