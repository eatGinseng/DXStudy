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
	result = InitializeShader(device, hwnd, L"../Engine/Color.vs", L"../Engine/Color.ps");
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
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
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
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, *errorMessage, NULL);
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
}