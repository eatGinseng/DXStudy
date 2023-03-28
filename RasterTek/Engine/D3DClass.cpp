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

	// DirectX Graphics interface factory 생성
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// Video card를 위한 Adapter를 생성하는 데 factory를 활용한다.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// adapter monitor, 즉 monitor를 Enumerate 함
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 모니터에서 DXGI_FORMAT_R8G8B8A8_UNORM 디스플레이 포맷에 맞는 모드 갯수를 얻음
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 이 모니터와 비디오 카드 조합으로 가능한 모든 디스플레이 모드 리스트를 만든다
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 디스플레이 모드 스트럭처를 채운다
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// 모든 모드를 돌면서 현재 screen width와 height에 맞는 것을 찾는다.
	// 찾으면 해당 모니터의 주사율의 numerator와 denomoiator를 저장한다.
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

	// 비디오 카드의 이름과 메모리 크기를 얻는다.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// 비디오 카드 전용 메모리를 MB 단위로 저장
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 비디오 카드 이름은 character 배열에 저장한다.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// 필요한 정보(numerator, denominator, 비디오 카드 정보) 를 얻었으니, 사용한 구조체와 인터페이스들은 해제해도 된다.

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

	// 보통의 32비트 surface를 back buffer에 지정
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// swap chain의 refresh rate 지정. back buffer가 front buffer에 그려지는 게 1초에 몇 번인지 결정.
	// VSync와 관련 : false일 경우, 시스템에 걸린 제한 없이 가능한 자주 업데이트. artifacts 있을 수 있음
	// back buffer의 refresh rate 설정
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

	// back buffer의 usage 설정
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더할 windows의 핸들 셋팅
	swapChainDesc.OutputWindow = hwnd;

	// multisampling 을 끈다.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 풀스크린 모드인지 아닌지?
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

	// 어느 버전의 DX를 상요할지 셜정. 11.0은 DirectX 11을 의미. 더 낮은 버전을 지원하려면 이걸 낮추면 돤다.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 이제 SwapChain을 생성할 준비 완료.
	// Direct3D device, Direct3D Device Context는 모든 Direct3D 함수들의 인터페인스임 = 아주 중요
	
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	
	// 비디오 카드가 DX11을 지원하지 않을 경우, 이 함수 호출은 실패한다.
	// 만약 D3D_DRIVER_TYPE_REFERENCE 로 한다면, DX는 CPU를 사용한다. 

	if (FAILED(result))
	{
		return false;
	}
	
	// 어떤 사용자들은 primary card가 dx11을 지원하지 않을 수 있다. 이럴때는 default device를 사용하지 말고, 
	// 머신의 모든 card를 enumerate 한 후, 유저가 선택한 카드로 device와 device Context를 생성하도록 해야 한다.

	// 이제 back buffer의 포인터를 얻어 swapchain에 붙일 것이다. 붙일 때는 CreateRenderTargetView() 함수를 사용한다.

	// back buffer의 포인터를 얻음
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// back buffer pointer로 renderTargetView를 생성한다.
	result = m_device->CreateRenderTargetView(BackBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// back buffer의 포인터는 더이상 사용하지 않으므로 해제해 준다.
	BackBufferPtr->Release();
	BackBufferPtr = 0;

	// Depth Buffer description을 생성한다. 폴리곤들이 3d 공간 안에 표현되려면 필요하다.
	// stencil buffer도 depth buffer에 붙여 줄 것이다. 모션블러 ,볼류매트릭 섀도 등의 효과에 필요하다.

	// depth buffer의 description을 초기화한다.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// depth buffer의 description을 채운다.
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

	// buffer는 단순한 2D 텍스처이기 때문에, buffer를 만들 때는 CreateTexture2D 함수를 사용한다.
	// 폴리곤이 소팅되고, rasterize 되면, 2D 버퍼상의 색깔 있는 픽셀이 되기 때문이다. 그 버퍼가 화면에 그려진다.

	// description을 사용해 depth buffer를 만든다.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	
	// Texture2D를 생성하는것과 RenderTargetView를 생성하는건 다른건가..?
	// 아까 backBuffer를 생성할 때는 renderTargetView를 생성했다. 별도의 description이 필요하지도 않았다.

	if (FAILED(result))
	{
		return false;
	}

	// stencil description 생성. 각 픽셀마다 어느 타입의 depth test를 수행할 것인지 결정한다.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀이 front face일 경우 Stencil 수행
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀이 back face일 경우 Stencil 수행
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// stencil state 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// depth stencil state를 셋팅. 이때는 deviceContext를 사용함에 유의
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 이제 만들어진 state로 depthStencilView를 생성할 수 있다.
	// 이 depthStencilView를 위한 description을 만든다. 그래야 Direct3D가 이 depth buffer를 depth stencil 텍스처로
	// 사용할 수 있다. description을 채운 후에 CreateDepthStencilView()를 호출한다.

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 이제 render target view와 depth stencil view를 render pipeline에 묶는다.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 이 밑으로는 extra function들.
	// 폴리곤을 어떻게 렌더링 할 것인지 컨트롤 가능한 rasterizer state를 만든다.
	// (와이어프레임으로 렌더링하거나 양면으로 렌더링 가능)
	// 만들지 않아도 기본적으로 아래와 같은 셋팅으로 생성되어 있지만, 직접 생성해야 컨트롤 가능함
	
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

	// viewport를 셋팅한다. 렌더타겟 스페이스에 clip space 좌표를 맵핑할 수 있게..
	// viewport 크기는 윈도우 전체 사이즈와 같게 한다.
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

	// projection matrix 만들기. fov, screen 종횡비, near, depth로 투영행렬 게산 가능
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// world matrix는 단위 행렬로 초기화
	m_worldMatrix = XMMatrixIdentity();

	// view matrix 생성은 생략. 카메라 클래스를 만들 것이므로

	// 2D 렌더링을 위한 직교투영행렬 (orthogonal projection matrix) 생성
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, (float)screenNear, (float)screenDepth);

	return true;

}

// Direct3D를 shutdown 할 때, 강제로 fullscreen모드 종료 : 그렇지 않으면 swap chain 을 release 할 때 exception이 생김
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


// beginScene : buffer를 초기화하고 비워서 그리기 좋은 상태로 만든다.
// endScene : 각 프레임의 끝에 그려진 내용을 표시하도록 swap chain에 지시한다.

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// 버퍼를 클리어할 색상을 정한다.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// back buffer를 클리어한다.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// depth buffer도 클리어한다.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;

}

void D3DClass::EndScene()
{

	// refresh rate에 따라 back buffer를 화면에 present
	if (m_vsync_enabled)
	{
		// 화면의 refresh rate에 맞춘다.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// 가능한 한 빠르게 present
		m_swapChain->Present(0, 0);
	}
	return;
}

// Pointer를 얻게 해 주는 helper functions

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

// projection world ortho matrix를 얻는 helpers

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

// video card 의 이름과 전용 메모리 용량, 디버깅할 때 필요
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

