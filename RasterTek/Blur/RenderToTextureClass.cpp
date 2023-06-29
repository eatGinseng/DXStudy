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

// ���� �� �ؽ�ó�� ����� ���� width �� height�� �޴´�.
// ��ũ�� ������ �������� ���, ��ũ���� ��Ⱦ���� ����� �Ѵ�. �׷��� ������ �ְ��� �����.
// �ؽ�ó�� Description�� ������ ��, �ؽ�ó�� �����Ѵ�. �׸��� �� �ؽ�ó�� ����Ÿ�� view�� ������ �ؽ�ó�� �׷��� �� �ֵ��� �Ѵ�.
// �׸��� ���� shader resource view�� ������ ���� �� �����͵��� ���ϴ� ������Ʈ�鿡�� ����� �� �ֵ��� �Ѵ�.

bool RenderToTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
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

	// render Target texture ����
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);

	// render target view�� description�� �¾�
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

	m_width = textureWidth;
	m_height = textureHeight;

	// Setup the viewport for rendering.
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// Setup the projection matrix.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(((float)XM_PI / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)textureWidth, (float)textureHeight, screenNear, screenDepth);

	return true;

}

int RenderToTextureClass::GetTextureWidth()
{
	return m_width;
}

int RenderToTextureClass::GetTextureHeight()
{
	return m_height;
}

void RenderToTextureClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
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

// SetRenderTarget �Լ������� ���� ��� �������� rendertarget�� �ص��� ���� �ش�.
void RenderToTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// output render pipeline�� render target view�� depth stencil buffer�� ���´�.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);

	return;
}

// Clear RenderTarget�� D3DClass�� BeginScene �Լ��� ���������, �� Ŭ������ RenderTarget���� �����Ѵ�.
// �� ����Ÿ�� �信 �������ϱ� ���� �� ������ ȣ��Ǿ�� �Ѵ�.
void RenderToTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha)
{
	float color[4];

	// color�� buffer�� setup
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

