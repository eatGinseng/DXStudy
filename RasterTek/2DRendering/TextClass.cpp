#include "TextClass.h"


TextClass::TextClass()
{
	m_Font = 0;
	m_FontShader = 0;

	m_sentence1 = 0;
	m_sentence2 = 0;

}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char mouseString[16];
	bool result;

	// mouseX integer�� string �������� ��ȯ
	_itoa_s(mouseX, tempString, 10);

	// mouseX string �¾�
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// ���ο� string ������ sentence vertex buffer�� ������Ʈ
	result = UpdateSentence(m_sentence1, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// Convert the mouseY integer to string format.
	_itoa_s(mouseY, tempString, 10);

	// Setup the mouseY string.
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;

}


bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight,
	XMMATRIX baseViewMatrix)
{
	bool result;

	// store the screen width and height
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// base view matrix ����
	m_baseViewMatrix = baseViewMatrix;

	// font object ����
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// font object �ʱ�ȭ
	char textureFilename[128];
	strcpy_s(textureFilename, "font.tga");
	// device, deviceContext, FontData TextFile path, Font Texture DDS File path
	result = m_Font->Initialize(device, deviceContext, L"fontdata.txt", textureFilename, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// ù ��° ���� �ʱ�ȭ
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}


	char sentence1[128];
	strcpy_s(sentence1, "Hello");

	// ������ Vertex Buffer�� ���ο� ��Ʈ�� ������ ä���.
	result = UpdateSentence(m_sentence1, sentence1, 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// �� ��° ���� �ʱ�ȭ
	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	char sentence2[128];
	strcpy_s(sentence2, "Goodbye");


	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, sentence2, 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;

}

void TextClass::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&m_sentence1);

	// Release the second sentence.
	ReleaseSentence(&m_sentence2);

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the font object.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	return;

}

// render �Լ��� ���� �� ���� ȭ�鿡 �׸���.
bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	// ù ���� �׸���
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	// �� ��° ���� �׸���
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

// InitializeSentence �Լ������� �� Vertex buffer�� SentenceType�� �����Ѵ�. �� ���۰� sentence�� �����ϰ� ������ �� ���δ�.
// maxLength �� ���� ��ǲ�� vertex buffer�� ũ�⸣ �����Ѵ�.
// ��� Sentence�� �� �Լ��κ��� �ʱ�ȭ �� ������ Index Buffer�� Vertex Buffer�� ������.
bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// sentence object ����
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// sentence buffer���� ��� null��
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// sentence�� �ִ� ���� ����
	(*sentence)->maxLength = maxLength;

	// vertex array�� vertex ���� ����
	(*sentence)->vertexCount = 6 * maxLength;

	// index array�� index ���� ����
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// vertex array ����
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// index array ����
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// vertex array�� 0���� �ʱ�ȭ
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// index array �ʱ�ȭ
	for (i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// dynamic vertex buffer description ����
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresoutce structure�� vertex data �����͸� ����
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// vertex buffer ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// index buffer�� �Ϲ����� static buffer�� �����Ѵ�.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// subresource structure�� index data�� �����͸� ����
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;

}

// UpdateSentence �Լ��� vertex buffer�� ������ ��ǲ���� ���� sentence�� �ٲ��ش�.
// vertex buffer�� ������ ������Ʈ �ϱ� ���� Map, Unmap, �׸��� memcpy �Լ��� ����Ѵ�.
bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// sentence �� �� ����
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// sentence ���� ���� ������ ���Ѵ�.
	numLetters = (int)strlen(text);

	// buffer overflow üũ
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// vertex array ����
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;

	}

	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// �׸��� ������ ��ũ������ ������ X, Y�� ���
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// FontClass�� sentence ������ vertex array ����
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// vertex array ������ sentence vertex buffer�� �����Ѵ�.
	// vertex buffer�� ��ż� ������ �� �� �ְ� �Ѵ�.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// vertex buffer�� data ���� �����͸� ��´�.
	verticesPtr = (VertexType*)mappedResource.pData;

	// vertex buffer �� �����͸� ������ �ִ´�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// vertex buffer ��� ����
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// vertex array ����
	delete[] vertices;
	vertices = 0;

	return true;

}

// sentence �ڽŰ� vertex, index buffer�� ����
void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// Release the sentence vertex buffer.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete* sentence;
		*sentence = 0;

	}


	return;
}

// RenderSentence �Լ��� vertex�� index buffer�� input assembler�� �ְ�,
// FontShaderClass object���� �־��� Sentence�� �����ϵ��� �Ѵ�.
// ������ view matrix�� �ƴ�, m_baseMatrix�� ���� �Ϳ� ��������. �̷��� �ϸ� ������ view �� �� �����Ӹ��� ���ϴ���,
// ��ũ������ ���� ��ġ�� �ؽ�Ʈ�� �׸� �� �ִ�. 
// ����, 2d ������ �׷����� �ϱ� ������  projection matrix�� �ƴ� ortho Matrix�� ����Ѵ�.

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	XMVECTOR pixelColor;
	bool result;

	// vertec buffer�� stride�� offset�� �����Ѵ�.
	stride = sizeof(VertexType);
	offset = 0;

	// vertex buffer�� input assembler���� Ȱ�� ���·� �ξ�, ������ �� �ֵ��� �Ѵ�.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// input sencence color�� pixel color vector�� �����.
	pixelColor = XMVectorSet(sentence->red, sentence->blue, sentence->green, 1.0f);

	// font shader�� �����Ѵ�.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);
	if (!result)
	{
		return false;
	}

	return true;

}

