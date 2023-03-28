#include "ColorShader.h"

ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;

}


ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}


ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// 버텍스 셰이더와 픽셀 셰이더 초기화
	result = InitializeShader(device, hwnd, L"../Engine/Color.vs", L"../Engine/Color.ps");
	if (!result)
	{
		return false;
	}
	
	return true;
}

void ColorShaderClass::Shutdown()
{
	// vertex shader와 pixel shader, 그외 연관된 오브젝트들을 shutdown
	ShutdownShader();

	return;
}

// Render() 함수는 SetShaderParameters() 함수로 셰이더 파라미터를 셋한다. 
// 파라미터가 셋팅되면 RenderShader() 를 호출해서, HLSL shader로 초록 트라이앵글을 그린다.

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{

	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// 준비된 buffer들을 셰이더를 사용해 그린다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// 포인터 초기화
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// vertex shader 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// 셰이더 컴파일이 실패할 경우, errorMessage에 뭔가 써 넣는다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// error message가 없을 경우, 셰이더를 찾지 못하는 것임
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더 코드 컴파일
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, *errorMessage, NULL);
	if (FAILED(result))
	{
		// 셰이더 컴파일이 실패할 경우, errorMessage에 뭔가 써 넣는다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// error message가 없을 경우, 셰이더를 찾지 못하는 것임
		else
		{
			MessageBox(hwnd, psFilename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 버텍스 셰이더 만들기
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼로부터 픽셀 셰이더 만들기
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}
	// 셰이더로 넘겨줄 데이터의 레이아웃을 셋팅한다.
	// POSITION과 COLOR데이터를 넘겨줄 것이므로, 각각의 사이즈와 semantic name을 정해주어야 한다.
	// semantic name으로 레이아웃의 각 요소를 어떤 용도로 사용할 지 셰이더에서 알 수 있다.
	// position vector에는 DXGI_FORMAT_R32G32B32_FLOAT 형식을, color에는 DXGI_FORMAT_R32G32B32A32_FLOAT 을 사용할 것이다.
	// 데이터가 버퍼 내에서 어떻게 공백을 가질 것인지는 alignedByteBufferOffset 함수로 정한다.
	// 여기에서는 첫 12 byte는 position이고 다음 16 byte는 color 로 정했다. 각 요소가 어디에서부터 시작하는지도 알려준다.
	// 직접 값을 지정하는 대신 D3D11_APPEND_ALIGNED_ELEMENT 를 사용하면 알아서 스페이싱을 해 준다.

	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃 안의 요소들의 갯수를 얻는다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// vertex input 레이아웃을 만든다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}
}