#include "Graphics.h"

#pragma comment(lib,"d3d11.lib")

Graphics::Graphics( HWND hWnd )
{
	RECT wndrect;
	GetWindowRect(hWnd, &wndrect);
	int width = wndrect.right - wndrect.left;
	int	height = wndrect.bottom - wndrect.top;

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// create device and front/back buffers, and swap chain and rendering context
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	);

	// gain access to texture subresource in swap chain (back buffer) in order to create a render target
	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer( 0,__uuidof(ID3D11Resource),reinterpret_cast<void**>(&pBackBuffer) );
	pDevice->CreateRenderTargetView(pBackBuffer,nullptr,&pTarget);
	pBackBuffer->Release();

	// create depth stensil state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ID3D11DepthStencilState* pDSState;
	pDevice->CreateDepthStencilState(&dsDesc, &pDSState);

	// bind depth state
	pContext->OMSetDepthStencilState(pDSState, 1u);

	// create depth stensil texture
	ID3D11Texture2D* pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 640u;
	descDepth.Height = 480u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	// create view of depth stensil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	pDevice->CreateDepthStencilView(
		pDepthStencil, &descDSV, &pDSV
	);

	// bind depth stensil view to OM
	pContext->OMSetRenderTargets(1u, &pTarget, pDSV);

	//set the primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//loading the vertex shader
	std::ifstream vertexShaderFile("VertexShader.cso", std::ios::binary);
	vertexShaderFile.seekg(0, std::ios::end);
	const int vfilesize = vertexShaderFile.tellg();
	vertexShaderFile.seekg(0, std::ios::beg);
	char* vfilebytedata = new char[vfilesize];
	int i = 0;
	for (char byte = vertexShaderFile.get(); vertexShaderFile.good(); byte = vertexShaderFile.get()) {
		vfilebytedata[i] = byte;
		i++;
	}
	ID3D11VertexShader* vertexShader = nullptr;
	pDevice->CreateVertexShader((void*)vfilebytedata, vfilesize, nullptr, &vertexShader);
	pContext->VSSetShader(vertexShader, nullptr, 0u);
	
	//setting the input layout
	ID3D11InputLayout* inputLayout = nullptr;
	D3D11_INPUT_ELEMENT_DESC* ild = new D3D11_INPUT_ELEMENT_DESC[2]{
		{"Position", 0u, DXGI_FORMAT_R32G32B32_FLOAT,0u,0u, D3D11_INPUT_PER_VERTEX_DATA, 0u},
		{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12u,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	pDevice->CreateInputLayout(ild, 2, vfilebytedata, vfilesize, &inputLayout);

	pContext->IASetInputLayout(inputLayout);

	//Create constant buffer and bind it to the vertex shader
	ConstantBuffer cb{};
	ID3D11Buffer* pConstantBuffer;
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(cb);
	cbd.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);
	pContext->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
	
	delete[] vfilebytedata;

	//loading the pixel shader
	std::ifstream pixelShaderFile("PixelShader.cso", std::ios::binary);
	pixelShaderFile.seekg(0, std::ios::end);
	const int pfilesize = pixelShaderFile.tellg();
	pixelShaderFile.seekg(0, std::ios::beg);
	char* pfilebytedata = new char[pfilesize];
	i = 0;
	for (char byte = pixelShaderFile.get(); pixelShaderFile.good(); byte = pixelShaderFile.get()) {
		pfilebytedata[i] = byte;
		i++;
	}
	ID3D11PixelShader* pixelShader = nullptr;
	pDevice->CreatePixelShader((void*)pfilebytedata, pfilesize, nullptr, &pixelShader);
	pContext->PSSetShader(pixelShader, nullptr, 0u);

	delete[] pfilebytedata;

	//setting the viewport
	D3D11_VIEWPORT vp = {0};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = width;
	vp.Height = height;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	
	pContext->RSSetViewports(1u, &vp);
}

Graphics::~Graphics()
{
	if( pTarget != nullptr )
	{
		pTarget->Release();
	}
	if( pContext != nullptr )
	{
		pContext->Release();
	}
	if( pSwap != nullptr )
	{
		pSwap->Release();
	}
	if( pDevice != nullptr )
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	pSwap->Present( 1u,0u );
}

void Graphics::DrawTrig(Vec2<float> v1, Vec2<float> v2, Vec2<float> v3) {
	const Vec3<float> vertices[3] = {Vec3<float>(v1,0.0f), Vec3<float>(v2,0.0f), Vec3<float>(v3,0.0f)};

	//creating the vertex buffer
	D3D11_BUFFER_DESC bd = {0};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(vertices);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof(Vec3<float>);

	D3D11_SUBRESOURCE_DATA subdata = {0};
	subdata.pSysMem = vertices;

	ID3D11Buffer* pVertexBuffer = nullptr;

	pDevice->CreateBuffer(&bd, &subdata, &pVertexBuffer);

	const UINT stride = sizeof(Vec3<float>);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);

	pContext->Draw(3u, 0u);
}

void Graphics::ClearBuffer( float red,float green,float blue ) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView( pTarget,color );
	pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawMesh(Mesh m, unsigned int r, unsigned int g, unsigned int b) {

	struct Vertex {
		float x;
		float y;
		float z;
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	std::vector<Vertex> outvertices;

	for (int i = 0; i < m.vertices.size(); i++) {
		Vertex nv;
		nv.x = m.vertices[i].x;
		nv.y = m.vertices[i].y;
		nv.z = m.vertices[i].z;

		nv.r = r;
		nv.g = g;
		nv.b = b;
		outvertices.push_back(nv);
	}

	ID3D11Buffer* pIndexBuffer;
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = m.indices.size() * sizeof(unsigned int);
	ibd.StructureByteStride = sizeof(unsigned int);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = &(m.indices[0]);
	pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

	// bind index buffer
	pContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0u);

	pIndexBuffer->Release();

	//creating the vertex buffer
	D3D11_BUFFER_DESC bd = { 0 };
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = m.vertices.size() * sizeof(Vertex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subdata = { 0 };
	subdata.pSysMem = &(outvertices[0]);

	ID3D11Buffer* pVertexBuffer = nullptr;

	pDevice->CreateBuffer(&bd, &subdata, &pVertexBuffer);

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	pContext->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);

	pVertexBuffer->Release();

	pContext->DrawIndexed(m.indices.size(), 0, 0);
}

void Graphics::SetConstantBuffer(void* rdata, int mode) {
	ConstantBuffer myData;

	// Retrieve the existing constant buffer
	ID3D11Buffer* constantBuffer;
	pContext->VSGetConstantBuffers(0, 1, &constantBuffer);

	// Update the constant buffer data
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ConstantBuffer* dataPtr = (ConstantBuffer*)mappedResource.pData;
	switch (mode) {
	case 0:
	{
		float zdis = *(float*)rdata;
		dataPtr->matrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0, 0, zdis) * DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f));
		break;
	}
	case 1:
	{
		struct datastruct{ float time; float zdis; };
		datastruct data = *(datastruct*)rdata;
		dataPtr->matrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationZ(data.time) * DirectX::XMMatrixRotationY(data.time / 2) * DirectX::XMMatrixTranslation(0, 0, data.zdis) * DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f));
		break;
	}
	}
	pContext->Unmap(constantBuffer, 0);
}