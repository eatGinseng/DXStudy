#include "D3DClass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;

}

D3DClass::D3DClass(const D3DClass& other)
{

}

D3DClass::~D3DClass()
{

}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* BackBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	// Store the vsync setting
	m_vsync_enabled = vsync;

	// DirectX Graphics interface factory ����
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// Video card�� ���� Adapter�� �����ϴ� �� factory�� Ȱ���Ѵ�.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// adapter monitor, �� monitor�� Enumerate ��
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// ����Ϳ��� DXGI_FORMAT_R8G8B8A8_UNORM ���÷��� ���˿� �´� ��� ������ ����
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// �� ����Ϳ� ���� ī�� �������� ������ ��� ���÷��� ��� ����Ʈ�� �����
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// ���÷��� ��� ��Ʈ��ó�� ä���
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// ��� ��带 ���鼭 ���� screen width�� height�� �´� ���� ã�´�.
	// ã���� �ش� ������� �ֻ����� numerator�� denomoiator�� �����Ѵ�.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{	
				// ????
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// ���� ī���� �̸��� �޸� ũ�⸦ ��´�.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ī�� ���� �޸𸮸� MB ������ ����
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ���� ī�� �̸��� character �迭�� �����Ѵ�.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// �ʿ��� ����(numerator, denominator, ���� ī�� ����) �� �������, ����� ����ü�� �������̽����� �����ص� �ȴ�.

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// initialize the swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// set to a single back buffer
	swapChainDesc.BufferCount = 1;

	// set the width and heignt of the back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// ������ 32��Ʈ surface�� back buffer�� ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// swap chain�� refresh rate ����. back buffer�� front buffer�� �׷����� �� 1�ʿ� �� ������ ����.
	// VSync�� ���� : false�� ���, �ý��ۿ� �ɸ� ���� ���� ������ ���� ������Ʈ. artifacts ���� �� ����
	// back buffer�� refresh rate ����
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// back buffer�� usage ����
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// ������ windows�� �ڵ� ����
	swapChainDesc.OutputWindow = hwnd;

	// multisampling �� ����.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Ǯ��ũ�� ������� �ƴ���?
	if (!fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// ��� ������ DX�� ������� ����. 11.0�� DirectX 11�� �ǹ�. �� ���� ������ �����Ϸ��� �̰� ���߸� �ô�.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// ���� SwapChain�� ������ �غ� �Ϸ�.
	// Direct3D device, Direct3D Device Context�� ��� Direct3D �Լ����� �������ν��� = ���� �߿�
	
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	
	// ���� ī�尡 DX11�� �������� ���� ���, �� �Լ� ȣ���� �����Ѵ�.
	// ���� D3D_DRIVER_TYPE_REFERENCE �� �Ѵٸ�, DX�� CPU�� ����Ѵ�. 

	if (FAILED(result))
	{
		return false;
	}
	
	// � ����ڵ��� primary card�� dx11�� �������� ���� �� �ִ�. �̷����� default device�� ������� ����, 
	// �ӽ��� ��� card�� enumerate �� ��, ������ ������ ī��� device�� device Context�� �����ϵ��� �ؾ� �Ѵ�.

	// ���� back buffer�� �����͸� ��� swapchain�� ���� ���̴�. ���� ���� CreateRenderTargetView() �Լ��� ����Ѵ�.

	// back buffer�� �����͸� ����
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// back buffer pointer�� renderTargetView�� �����Ѵ�.
	result = m_device->CreateRenderTargetView(BackBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// back buffer�� �����ʹ� ���̻� ������� �����Ƿ� ������ �ش�.
	BackBufferPtr->Release();
	BackBufferPtr = 0;

	// Depth Buffer description�� �����Ѵ�. ��������� 3d ���� �ȿ� ǥ���Ƿ��� �ʿ��ϴ�.
	// stencil buffer�� depth buffer�� �ٿ� �� ���̴�. ��Ǻ� ,������Ʈ�� ���� ���� ȿ���� �ʿ��ϴ�.

	// depth buffer�� description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// depth buffer�� description�� ä���.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// buffer�� �ܼ��� 2D �ؽ�ó�̱� ������, buffer�� ���� ���� CreateTexture2D �Լ��� ����Ѵ�.
	// �������� ���õǰ�, rasterize �Ǹ�, 2D ���ۻ��� ���� �ִ� �ȼ��� �Ǳ� �����̴�. �� ���۰� ȭ�鿡 �׷�����.

	// description�� ����� depth buffer�� �����.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	
	// Texture2D�� �����ϴ°Ͱ� RenderTargetView�� �����ϴ°� �ٸ��ǰ�..?
	// �Ʊ� backBuffer�� ������ ���� renderTargetView�� �����ߴ�. ������ description�� �ʿ������� �ʾҴ�.

	if (FAILED(result))
	{
		return false;
	}

	// stencil description ����. �� �ȼ����� ��� Ÿ���� depth test�� ������ ������ �����Ѵ�.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// �ȼ��� front face�� ��� Stencil ����
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ��� back face�� ��� Stencil ����
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// stencil state ����
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// depth stencil state�� ����. �̶��� deviceContext�� ����Կ� ����
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// ���� ������� state�� depthStencilView�� ������ �� �ִ�.
	// �� depthStencilView�� ���� description�� �����. �׷��� Direct3D�� �� depth buffer�� depth stencil �ؽ�ó��
	// ����� �� �ִ�. description�� ä�� �Ŀ� CreateDepthStencilView()�� ȣ���Ѵ�.

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// ���� render target view�� depth stencil view�� render pipeline�� ���´�.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// �� �����δ� extra function��.
	// �������� ��� ������ �� ������ ��Ʈ�� ������ rasterizer state�� �����.
	// (���̾����������� �������ϰų� ������� ������ ����)
	// ������ �ʾƵ� �⺻������ �Ʒ��� ���� �������� �����Ǿ� ������, ���� �����ؾ� ��Ʈ�� ������
	
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	m_deviceContext->RSSetState(m_rasterState);

	// viewport�� �����Ѵ�. ����Ÿ�� �����̽��� clip space ��ǥ�� ������ �� �ְ�..
	// viewport ũ��� ������ ��ü ������� ���� �Ѵ�.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_deviceContext->RSSetViewports(1, &viewport);

	// set up projection matrix

	fieldOfView = (float) XM_PI/ 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// projection matrix �����. fov, screen ��Ⱦ��, near, depth�� ������� �Ի� ����
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// world matrix�� ���� ��ķ� �ʱ�ȭ
	m_worldMatrix = XMMatrixIdentity();

	// view matrix ������ ����. ī�޶� Ŭ������ ���� ���̹Ƿ�

	// 2D �������� ���� ����������� (orthogonal projection matrix) ����
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, (float)screenNear, (float)screenDepth);

	return true;

}

// Direct3D�� shutdown �� ��, ������ fullscreen��� ���� : �׷��� ������ swap chain �� release �� �� exception�� ����
void D3DClass::Shutdown()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;


}


// beginScene : buffer�� �ʱ�ȭ�ϰ� ����� �׸��� ���� ���·� �����.
// endScene : �� �������� ���� �׷��� ������ ǥ���ϵ��� swap chain�� �����Ѵ�.

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// ���۸� Ŭ������ ������ ���Ѵ�.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// back buffer�� Ŭ�����Ѵ�.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// depth buffer�� Ŭ�����Ѵ�.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;

}

void D3DClass::EndScene()
{

	// refresh rate�� ���� back buffer�� ȭ�鿡 present
	if (m_vsync_enabled)
	{
		// ȭ���� refresh rate�� �����.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// ������ �� ������ present
		m_swapChain->Present(0, 0);
	}
	return;
}

// Pointer�� ��� �� �ִ� helper functions

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

// projection world ortho matrix�� ��� helpers

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

// video card �� �̸��� ���� �޸� �뷮, ������� �� �ʿ�
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

