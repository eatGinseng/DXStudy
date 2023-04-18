#pragma once

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

// 이 헤더에서는 모듈을 사용할 때 필요한 라이브러리를 지정한다.
// 이 라이브러리에는 3D 그래픽을 띄우기 위한 DiretX 기능들이나, 모니터의 주사율, 사용중인 비디오 카드 등을 다루기 위한 도구 등이 포함된다.
// DX 10 라이브러리가 일부 아직 사용된다 - 11에서 업그레이드 되지 않았기 때문.

// Linking
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
// #pragma comment(lib, "d3dx11.lib")
// #pragma comment(lib, "d3dx10.lib")

// include

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
// 아래는 사라진듯
// #include <d3dx10math.h>
#include <DirectXMath.h>

#include <iostream>
#include <stdlib.h>

using namespace DirectX;


class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();
	

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

	ID3D11DepthStencilState* m_depthDisabledStencilState;

	// alpha blending을 위한 state 두 가지
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

};

#endif