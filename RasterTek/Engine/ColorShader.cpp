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

	// ���ؽ� ���̴��� �ȼ� ���̴� �ʱ�ȭ

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
	// vertex shader�� pixel shader, �׿� ������ ������Ʈ���� shutdown
	ShutdownShader();

	return;
}

// Render() �Լ��� SetShaderParameters() �Լ��� ���̴� �Ķ���͸� ���Ѵ�. 
// �Ķ���Ͱ� ���õǸ� RenderShader() �� ȣ���ؼ�, HLSL shader�� �ʷ� Ʈ���̾ޱ��� �׸���.

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{

	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// �غ�� buffer���� ���̴��� ����� �׸���.
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

	// ������ �ʱ�ȭ
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// vertex shader ������
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� �������� ������ ���, errorMessage�� ���� �� �ִ´�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// error message�� ���� ���, ���̴��� ã�� ���ϴ� ����
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� �ڵ� ������
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� �������� ������ ���, errorMessage�� ���� �� �ִ´�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// error message�� ���� ���, ���̴��� ã�� ���ϴ� ����
		else
		{
			MessageBox(hwnd, psFilename, L"Missing shader file", MB_OK);
		}

		return false;
	}

	// ���۷κ��� ���ؽ� ���̴� �����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���۷κ��� �ȼ� ���̴� �����
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
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
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// vertex Shader buffer, index Shader buffer�� �� �̻� �ʿ�����Ƿ� ������ �ش�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	vertexShaderBuffer = 0;

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


	return true;
}

// Shutdown Shader �Լ��� InitializeShader() �Լ����� �¾��ߴ� �� ���� �������̽����� �����Ѵ�.

void ColorShaderClass::ShutdownShader()
{

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

	// vertex shader�� �����Ѵ�.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;

	}

	return;

}

// OutputErrorMessage() �Լ��� ���ؽ� ���̴��� �ȼ� ���̴��� �������� �� ������ ���� �޽������� ���´�.
void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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
	for (int i = 0; i < bufferSize; i++)
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

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
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

	// ���� ������Ʈ �� constant buffer�� vertex shader �� ������ �ش�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

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
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ�� ������
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}