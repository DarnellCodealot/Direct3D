#pragma once
#include <windows.h>
#include <d3d11.h>
#include <fstream>
#include <cstdlib>
#include <DirectXMath.h>
#include "vec.h"
#include "mesh.h"

class Graphics
{
private:
	//structure of constant buffer
	struct ConstantBuffer {
		DirectX::XMMATRIX matrix;
	};
public:
	Graphics( HWND hWnd );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;
	~Graphics();
	void EndFrame();
	void DrawTrig(Vec2<float> v1, Vec2<float> v2, Vec2<float> v3);//you need clockwise winding
	void DrawMesh(Mesh m, unsigned int r, unsigned int g, unsigned int b);
	void ClearBuffer( float red,float green,float blue ) noexcept;
	void SetConstantBuffer(void* rdata, int mode);
public:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
	ID3D11DepthStencilView* pDSV = nullptr;
};