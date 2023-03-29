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

	WCHAR vs[19] = {};
	WCHAR ps[19] = {};

	wcsncpy_s(vs, L"../Engine/color.vs", 19);
	wcsncpy_s(ps, L"../Engine/color.ps", 19);

	result = InitializeShader(device, hwnd, vs, ps);
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
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
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
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
		&pixelShaderBuffer, &errorMessage);
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

	// vertex Shader buffer, index Shader buffer는 더 이상 필요없으므로 해제해 준다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	vertexShaderBuffer = 0;

	// 이제 마지막으로 셰이더에서 사용할 constant buffer를 셋팅해 준다. 여기에서는 1개의 cbuffer만 존재한다.
	// 버퍼 usage는 dynamic이어야 한다. 왜냐하면 매 프레임마다 업데이트 될 것이기 때문이다.
	// bind flag를 보면, 이 버퍼가 constant buffer로 사용될 것임을 알 수 있다.
	// CPU Access flag는 usage와 맞춰야 하므로 D3D11_CPU_ACCESS_WRITE 여야 한다.
	// 이렇게 description을 채운 후에는 constant buffer를 만들 수 있고, 
	// SetShaderParamters() 함수로 셰이더의 내부 변수들에 접근할 수 있게 된다.

	// 버넥스 셰이더에 있는 dynamic matrix constant buffer를 위한 description을 셋업한다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// constant buffer의 포인터를 만들어 버텍스 셰이더의 constant buffer에 접근할 수 있도록 한다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

// Shutdown Shader 함수는 InitializeShader() 함수에서 셋업했던 네 가지 인터페이스들을 해제한다.

void ColorShaderClass::ShutdownShader()
{

	// matrix constant buffer를 release한다.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// layout을 해제한다.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// pixel shader를 해제한다.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// vertex shader를 해제한다.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;

	}

	return;

}

// OutputErrorMessage() 함수는 버텍스 셰이더나 픽셀 셰이더를 컴파일할 때 나오는 에러 메시지들을 적는다.
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{

	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// 에러 메시지 텍스트 버퍼로의 포인터를 얻는다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// error message의 길이를 얻는다.
	bufferSize = errorMessage->GetBufferSize();

	// file을 열어 error message를 적는다.
	fout.open("shader-error.txt");
	for (int i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// file 닫기
	fout.close();

	// error messsages 를 해제
	errorMessage->Release();
	errorMessage = 0;

	// 메시지창을 띄워서 사용자가 컴파일 에러를 텍스트 파일에서 확인하도록 유도
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;

}

// SetShaderParameters() 함수는 셰이더 안의 전역변수들을 쉽게 세팅할 수 있도록 한다.
// 여기에서 사용되는 행렬들은 GraphicsClass 클래스 안에서 만들어 질 것이다. 
// 그 이후에 이 함수가 호출되어, Render() 가 호출될 때 생성된 행렬들을 버텍스 셰이더로 보낸다.

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// 셰이더로 넘겨주기 전, 행렬들을 transpose 해 주어야 한다. DirectX11에서는 이 과정이 필수이다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// m_matrixBuffer를 잠그고, 안에 새로운 매트릭스를 쓴 뒤, 잠금을 푼다.
	// map() 함수는 GPU resource 메모리에 대한 포인터를 얻는 데 사용한다.
	// 반환된 메모리 포인터에 대응되는 메모리에서 CPU가 GPU Resource를 읽거나 쓸 수 있다.
	// CPU로부터 map 된 리소스에는 GPU는 접근할 수 없다.
	// D3D11_MAP_WRITE_DISCARD 는, CPU가 버퍼에 쓸 것이며, 버퍼에 있던 내용들은 버린다는 뜻이다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	};

	// constant buffer 내의 데이터에 대한 포인터를 얻는다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// constant buffer에 행렬들을 카피한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// constant buffer 잠금 해제
	// Unmap 하고 나면 CPU는 더 이상 반환된 메모리 포인터에 접근할 수 없다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// HLSL Vertex shader에 matrix buffer를 set 해 준다.
	// 버텍스 셰이더 안의 constant buffer 의 위치를 정해준다.
	bufferNumber = 0;

	// 값이 업데이트 된 constant buffer를 vertex shader 로 셋팅해 준다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

// render() 함수에서 불릴 두 번째 함수는 RenderShader() 이다.
// SetShaderParameters() 는 셰이더 파라미터를 제대로 셋팅하기 위해 먼저 호출된다.

// 먼저, input assembler 에 input layout를 활성화하도록 셋팅한다.
// 이렇게 하면, GPU 에서 vertex buffer 안에 있는 데이터들의 포맷을 알게 된다.
// 그리고 나서, 이 버텍스 버퍼를 그리기 위한 버텍스 셰이더와 픽셀 셰이더를 지정해 준다.
// 셰이더들이 정해지면, D3d DeviceContext로 DrawIndexed() 함수를 호출해 삼각형을 그릴 것이다.
// 이 함수가 호출되면, 초록색 삼각형이 렌더링된다.

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{

	// set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그리기 위한 버텍스 셰이더, 픽셀 셰이더를 지정
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형 렌더링
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}