////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "renderToTextureclass.h"

RenderToTextureClass::RenderToTextureClass()
{
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;
}


RenderToTextureClass::RenderToTextureClass(const RenderToTextureClass& other)
{
}


RenderToTextureClass::~RenderToTextureClass()
{
}

// 렌더 투 텍스처를 만들고 싶은 width 와 height를 받는다.
// 스크린 내용을 렌더링할 경우, 스크린의 종횡비응 맞춰야 한다. 그렇지 않으면 왜곡이 생긴다.
// 텍스처의 Description을 생성한 후, 텍스처를 생성한다. 그리고 그 텍스처를 렌더타겟 view로 설정해 텍스처가 그려질 수 있도록 한다.
// 그리고 나서 shader resource view를 생성해 렌더 된 데이터들을 원하는 오브젝트들에서 사용할 수 있도록 한다.

bool RenderToTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, int screenDepth, int screenNear)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// RenderTarget texture description initialize
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Render target texture description setup
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// render Target texture 생성
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);

	// render target view의 description을 셋업
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;


}

void RenderToTextureClass::Shutdown()
{



	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}

	return;
}

// SetRenderTarget 함수에서는 현재 모든 렌더링을 rendertarget에 해도록 셋해 준다.
void RenderToTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// output render pipeline에 render target view와 depth stencil buffer를 묶는다.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);

	return;
}

// Clear RenderTarget은 D3DClass의 BeginScene 함수를 모방하지만, 이 클래스의 RenderTarget에만 동작한다.
// 이 렌더타겟 뷰에 렌더링하기 전에 매 프레임 호출되어야 한다.
void RenderToTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha)
{
	float color[4];

	// color를 buffer에 setup
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// back buffer clear
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// depth buffer clear
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

ID3D11ShaderResourceView* RenderToTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}

