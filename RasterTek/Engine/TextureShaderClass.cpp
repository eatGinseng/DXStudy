#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{

	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;

	m_sampleState = 0;

}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}


TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// 버텍스 셰이더와 픽셀 셰이더 초기화
	result = InitializeShader(device, hwnd, L"../Engine/TextureShader.hlsl");
	if (!result)
	{
		return false;
	}

}

void TextureShaderClass::Shutdown()
{

	// 버텍스 셰이더, 픽셀 셰이더, 그리고 연관된 오브젝트들 해제
	ShutdownShader();

	return;

}

// 셰이더에 텍스처 리소스를 전해주기 위한 새로운 파라미터인 texture가 추가되었다.
// 이 파라미터는 SetShaderParameres() 함수에 전해져 텍스처를 셰이더에 전달, 렌더링에 사용될 수 있게 한다.
bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{

	bool result;

	// 렌더링 시 사용할 셰이더 파라미터를 셋팅한다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	// 준비된 버퍼들을 셰이더로 렌더링 한다.
	RenderShader(deviceContext, indexCount);

	return true;

}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, LPCWSTR shaderFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc;

	// Sampler를 저장하기 위한 변수
	D3D11_SAMPLER_DESC samplerDesc;

	// 포인터를 초기화
	errorMessage = 0;

	// vertex shader 컴파일
	result = D3DX11CompileFromFile(shaderFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBlob, &errorMessage, NULL);
	if (FAILED(result))
	{
		// 셰이더 컴파일이 실패할 경우, errorMessage에 뭔가 써 넣는다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, shaderFilename);
		}
		// error message가 없을 경우, 셰이더를 찾지 못하는 것임
		else
		{
			MessageBox(hwnd, shaderFilename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더 코드 컴파일
	result = D3DX11CompileFromFile(shaderFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		NULL, &pixelShaderBlob, &errorMessage, NULL);
	if (FAILED(result))
	{
		// 셰이더 컴파일이 실패할 경우, errorMessage에 뭔가 써 넣는다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, shaderFilename);
		}
		// error message가 없을 경우, 셰이더를 찾지 못하는 것임
		else
		{
			MessageBox(hwnd, shaderFilename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 버텍스 셰이더 만들기
	result = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼로부터 픽셀 셰이더 만들기
	result = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 두 번째 input layout의 두 번째 element가 Color에서 TEXCOORD로 semantic name이 바뀌었음.
	// 데이터 포맷은 그대로 DXGI_FORMAT_R32G32_FLOAT 임.

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBlob->Release();
	vertexShaderBlob = 0;

	pixelShaderBlob->Release();
	pixelShaderBlob = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// sampler state가 여기에서 정의된다. 이후 픽셀 셰이더에 넘겨저 사용할 수 있다.
	// 가장 중요한 부분은 Filter인데, 어느 픽셀이 사용될지, 혹은 합쳐져서 텍스처의 최종 룩을 만들지를 결정한다.
	//  여기에서는 가장 비싼 연산이지만 시각적인 결과물은 가장 좋은 것을 선택하였다.
	// D3D11_FILTER_MIN_MAG_MIP_LINEAR 는 샘플러가 확대, 축소, mip 레벨 샘플링시 선형 보간을 사용하도록 한다.

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 샘플러 스테이트 생성
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		return false;
	}
	return true;
}

void TextureShaderClass::ShutdownShader()
{

	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;

}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, LPCWSTR shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

// SetShaderParameters() 함수는 텍스처 리소스의 포인터를 받아 셰이더에 적용한다.
// 버퍼를 렌더링 하기 전, 텍스처를 미리 셋하는 것에 주의한다.

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// SetShaderParameters() 함수가 픽셀 셰이더의 텍스처를 포함하도록 변경되었다.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// sample state를 렌더링 이전에 픽셀 셰이더에 셋하도록 변경되었음
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 도형 그리기
	deviceContext->DrawIndexed(indexCount, 0, 0);

}