#include "ColorShaderClass.h"


ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;

	m_hullShader = 0;
	m_domainShader = 0;

	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_tessellationBuffer = 0;

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

	// ���ؽ� ���̴��� �ȼ� ���̴� �ʱ�ȭ
	result = InitializeShader(device, hwnd, L"colorShader.hlsl");
	if (!result)
	{
		return false;
	}
	
	return true;
}

void ColorShaderClass::Shutdown()
{
	// vertex shader�� pixel shader, �׿� ������ ������Ʈ���� shutdown
	ShutdownShader();

	return;
}

// Render() �Լ��� SetShaderParameters() �Լ��� ���̴� �Ķ���͸� ���Ѵ�. 
// �Ķ���Ͱ� ���õǸ� RenderShader() �� ȣ���ؼ�, HLSL shader�� �ʷ� Ʈ���̾ޱ��� �׸���.

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
{

	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, tessellationAmount);
	if (!result)
	{
		return false;
	}

	// �غ�� buffer���� ���̴��� ����� �׸���.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, LPCWSTR shaderFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;

	ID3D10Blob* hullShaderBuffer;
	ID3D10Blob* domainShaderBuffer;

	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;

	// ������ �ʱ�ȭ
	errorMessage = 0;
	vertexShaderBuffer = 0;

	hullShaderBuffer = 0;
	domainShaderBuffer = 0;

	pixelShaderBuffer = 0;

	// vertex shader ������
	result = D3DX11CompileFromFile(L"color.vs", NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBlob, &errorMessage, NULL);
	if (FAILED(result))
	{
		// ���̴� �������� ������ ���, errorMessage�� ���� �� �ִ´�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, L"color.vs");
		}
		// error message�� ���� ���, ���̴��� ã�� ���ϴ� ����
		else
		{
			MessageBox(hwnd, L"color.vs", L"Missing shader file", MB_OK);
		}

		return false;
	}

	// hull shader ������
	result = D3DX11CompileFromFile(L"color.hs", NULL, NULL, "ColorHullShader", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &hullShaderBlob, &errorMessage, NULL);
	if (FAILED(result))
	{
		// ���̴� �������� ������ ���, errorMessage�� ���� �� �ִ´�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, L"color.hs");
		}
		// error message�� ���� ���, ���̴��� ã�� ���ϴ� ����
		else
		{
			MessageBox(hwnd, L"color.hs", L"Missing shader file", MB_OK);
		}

		return false;
	}

	// Compile the domain shader code.
	result = D3DX11CompileFromFile(L"color.ds", NULL, NULL, "ColorDomainShader", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&domainShaderBlob, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, L"color.ds");
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, L"color.ds", L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� �ڵ� ������
	result = D3DX11CompileFromFile(L"color.ps", NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBlob, &errorMessage, NULL);
	if (FAILED(result))
	{
		// ���̴� �������� ������ ���, errorMessage�� ���� �� �ִ´�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, L"color.ps");
		}
		// error message�� ���� ���, ���̴��� ã�� ���ϴ� ����
		else
		{
			MessageBox(hwnd, L"color.ps", L"Missing shader file", MB_OK);
		}

		return false;
	}

	// ���۷κ��� ���ؽ� ���̴� �����
	result = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Hull, Domain Shader �����
	// Create the hull shader from the buffer.
	result = device->CreateHullShader(hullShaderBlob->GetBufferPointer(), hullShaderBlob->GetBufferSize(), NULL, &m_hullShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the domain shader from the buffer.
	result = device->CreateDomainShader(domainShaderBlob->GetBufferPointer(), domainShaderBlob->GetBufferSize(), NULL, &m_domainShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���۷κ��� �ȼ� ���̴� �����
	result = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}
	// ���̴��� �Ѱ��� �������� ���̾ƿ��� �����Ѵ�.
	// POSITION�� COLOR�����͸� �Ѱ��� ���̹Ƿ�, ������ ������� semantic name�� �����־�� �Ѵ�.
	// semantic name���� ���̾ƿ��� �� ��Ҹ� � �뵵�� ����� �� ���̴����� �� �� �ִ�.
	// position vector���� DXGI_FORMAT_R32G32B32_FLOAT ������, color���� DXGI_FORMAT_R32G32B32A32_FLOAT �� ����� ���̴�.
	// �����Ͱ� ���� ������ ��� ������ ���� �������� alignedByteBufferOffset �Լ��� ���Ѵ�.
	// ���⿡���� ù 12 byte�� position�̰� ���� 16 byte�� color �� ���ߴ�. �� ��Ұ� ��𿡼����� �����ϴ����� �˷��ش�.
	// ���� ���� �����ϴ� ��� D3D11_APPEND_ALIGNED_ELEMENT �� ����ϸ� �˾Ƽ� �����̽��� �� �ش�.

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

	// ���̾ƿ� ���� ��ҵ��� ������ ��´�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// vertex input ���̾ƿ��� �����.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// vertex Shader buffer, index Shader buffer�� �� �̻� �ʿ�����Ƿ� ������ �ش�.
	vertexShaderBlob->Release();
	vertexShaderBlob = 0;

	// hull Shader buffer�� �� �̻� �ʿ�����Ƿ� ������ �ش�.
	hullShaderBlob->Release();
	hullShaderBlob = 0;

	domainShaderBlob->Release();
	domainShaderBlob = 0;

	pixelShaderBlob->Release();
	pixelShaderBlob = 0;

	// ���� ���������� ���̴����� ����� constant buffer�� ������ �ش�. ���⿡���� 1���� cbuffer�� �����Ѵ�.
	// ���� usage�� dynamic�̾�� �Ѵ�. �ֳ��ϸ� �� �����Ӹ��� ������Ʈ �� ���̱� �����̴�.
	// bind flag�� ����, �� ���۰� constant buffer�� ���� ������ �� �� �ִ�.
	// CPU Access flag�� usage�� ����� �ϹǷ� D3D11_CPU_ACCESS_WRITE ���� �Ѵ�.
	// �̷��� description�� ä�� �Ŀ��� constant buffer�� ���� �� �ְ�, 
	// SetShaderParamters() �Լ��� ���̴��� ���� �����鿡 ������ �� �ְ� �ȴ�.

	// ���ؽ� ���̴��� �ִ� dynamic matrix constant buffer�� ���� description�� �¾��Ѵ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// constant buffer�� �����͸� ����� ���ؽ� ���̴��� constant buffer�� ������ �� �ֵ��� �Ѵ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic tessellation constant buffer that is in the hull shader.
	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the hull shader constant buffer from within this class.
	result = device->CreateBuffer(&tessellationBufferDesc, NULL, &m_tessellationBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

// Shutdown Shader �Լ��� InitializeShader() �Լ����� �¾��ߴ� �� ���� �������̽����� �����Ѵ�.

void ColorShaderClass::ShutdownShader()
{

	// Release the tessellation constant buffer.
	if (m_tessellationBuffer)
	{
		m_tessellationBuffer->Release();
		m_tessellationBuffer = 0;
	}

	// matrix constant buffer�� release�Ѵ�.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// layout�� �����Ѵ�.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// pixel shader�� �����Ѵ�.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the domain shader.
	if (m_domainShader)
	{
		m_domainShader->Release();
		m_domainShader = 0;
	}

	// Release the hull shader.
	if (m_hullShader)
	{
		m_hullShader->Release();
		m_hullShader = 0;
	}

	// vertex shader�� �����Ѵ�.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;

	}

	return;

}

// OutputErrorMessage() �Լ��� ���ؽ� ���̴��� �ȼ� ���̴��� �������� �� ������ ���� �޽������� ���´�.
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, LPCWSTR shaderFilename)
{

	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// ���� �޽��� �ؽ�Ʈ ���۷��� �����͸� ��´�.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// error message�� ���̸� ��´�.
	bufferSize = errorMessage->GetBufferSize();

	// file�� ���� error message�� ���´�.
	fout.open("shader-error.txt");
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// file �ݱ�
	fout.close();

	// error messsages �� ����
	errorMessage->Release();
	errorMessage = 0;

	// �޽���â�� ����� ����ڰ� ������ ������ �ؽ�Ʈ ���Ͽ��� Ȯ���ϵ��� ����
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;

}

// SetShaderParameters() �Լ��� ���̴� ���� ������������ ���� ������ �� �ֵ��� �Ѵ�.
// ���⿡�� ���Ǵ� ��ĵ��� GraphicsClass Ŭ���� �ȿ��� ����� �� ���̴�. 
// �� ���Ŀ� �� �Լ��� ȣ��Ǿ�, Render() �� ȣ��� �� ������ ��ĵ��� ���ؽ� ���̴��� ������.

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float tessellationAmount)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TessellationBufferType* dataPtr2;

	unsigned int bufferNumber;

	// ���̴��� �Ѱ��ֱ� ��, ��ĵ��� transpose �� �־�� �Ѵ�. DirectX11������ �� ������ �ʼ��̴�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// m_matrixBuffer�� ��װ�, �ȿ� ���ο� ��Ʈ������ �� ��, ����� Ǭ��.
	// map() �Լ��� GPU resource �޸𸮿� ���� �����͸� ��� �� ����Ѵ�.
	// ��ȯ�� �޸� �����Ϳ� �����Ǵ� �޸𸮿��� CPU�� GPU Resource�� �аų� �� �� �ִ�.
	// CPU�κ��� map �� ���ҽ����� GPU�� ������ �� ����.
	// D3D11_MAP_WRITE_DISCARD ��, CPU�� ���ۿ� �� ���̸�, ���ۿ� �ִ� ������� �����ٴ� ���̴�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	};

	// constant buffer ���� �����Ϳ� ���� �����͸� ��´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// constant buffer�� ��ĵ��� ī���Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// constant buffer ��� ����
	// Unmap �ϰ� ���� CPU�� �� �̻� ��ȯ�� �޸� �����Ϳ� ������ �� ����.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// HLSL Vertex shader�� matrix buffer�� set �� �ش�.
	// ���ؽ� ���̴� ���� constant buffer �� ��ġ�� �����ش�.
	bufferNumber = 0;

	// Finally set the matrix constant buffer in the domain shader with the updated values.
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Lock the tessellation constant buffer so it can be written to.
	result = deviceContext->Map(m_tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the tessellation constant buffer.
	dataPtr2 = (TessellationBufferType*)mappedResource.pData;

	// Copy the tessellation data into the constant buffer.
	dataPtr2->tessellationAmount = tessellationAmount;
	dataPtr2->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Unlock the tessellation constant buffer.
	deviceContext->Unmap(m_tessellationBuffer, 0);

	// Set the position of the tessellation constant buffer in the hull shader.
	bufferNumber = 0;

	// Now set the tessellation constant buffer in the hull shader with the updated values.
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_tessellationBuffer);

	return true;
}

// render() �Լ����� �Ҹ� �� ��° �Լ��� RenderShader() �̴�.
// SetShaderParameters() �� ���̴� �Ķ���͸� ����� �����ϱ� ���� ���� ȣ��ȴ�.

// ����, input assembler �� input layout�� Ȱ��ȭ�ϵ��� �����Ѵ�.
// �̷��� �ϸ�, GPU ���� vertex buffer �ȿ� �ִ� �����͵��� ������ �˰� �ȴ�.
// �׸��� ����, �� ���ؽ� ���۸� �׸��� ���� ���ؽ� ���̴��� �ȼ� ���̴��� ������ �ش�.
// ���̴����� ��������, D3d DeviceContext�� DrawIndexed() �Լ��� ȣ���� �ﰢ���� �׸� ���̴�.
// �� �Լ��� ȣ��Ǹ�, �ʷϻ� �ﰢ���� �������ȴ�.

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{

	// set the vertex input layout
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸��� ���� ���ؽ� ���̴�, �ȼ� ���̴��� ����
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);

	deviceContext->HSSetShader(m_hullShader, NULL, 0);
	deviceContext->DSSetShader(m_domainShader, NULL, 0);

	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ�� ������
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}