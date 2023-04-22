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

	// mouseX integer를 string 포맷으로 변환
	_itoa_s(mouseX, tempString, 10);

	// mouseX string 셋업
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// 새로운 string 정보로 sentence vertex buffer를 업데이트
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

	// base view matrix 저장
	m_baseViewMatrix = baseViewMatrix;

	// font object 생성
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// font object 초기화
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

	// 첫 번째 문장 초기화
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}


	char sentence1[128];
	strcpy_s(sentence1, "Hello");

	// 문장의 Vertex Buffer를 새로운 스트링 정보로 채운다.
	result = UpdateSentence(m_sentence1, sentence1, 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// 두 번째 문장 초기화
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

// render 함수로 문장 두 개를 화면에 그린다.
bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	// 첫 문장 그리기
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	// 두 번째 문장 그리기
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

// InitializeSentence 함수에서는 빈 Vertex buffer로 SentenceType를 생성한다. 이 버퍼가 sentence를 저장하고 렌더할 때 쓰인다.
// maxLength 로 들어온 인풋이 vertex buffer의 크기르 결정한다.
// 모든 Sentence는 이 함수로부터 초기화 된 각각의 Index Buffer와 Vertex Buffer를 가진다.
bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// sentence object 생성
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// sentence buffer들을 모두 null로
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// sentence의 최대 길이 셋팅
	(*sentence)->maxLength = maxLength;

	// vertex array에 vertex 갯수 저장
	(*sentence)->vertexCount = 6 * maxLength;

	// index array의 index 갯수 저장
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// vertex array 생성
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// index array 생성
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// vertex array를 0으로 초기화
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// index array 초기화
	for (i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// dynamic vertex buffer description 생성
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresoutce structure에 vertex data 포인터를 전달
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// vertex buffer 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// index buffer는 일반적인 static buffer로 생성한다.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// subresource structure에 index data의 포인터를 전달
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

// UpdateSentence 함수는 vertex buffer의 내용을 인풋으로 들어온 sentence로 바꿔준다.
// vertex buffer의 내용을 업데이트 하기 위해 Map, Unmap, 그리고 memcpy 함수를 사용한다.
bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// sentence 의 색 저장
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// sentence 안의 글자 갯수를 구한다.
	numLetters = (int)strlen(text);

	// buffer overflow 체크
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// vertex array 생성
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;

	}

	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// 그리기 시작할 스크린상의 포지션 X, Y를 계산
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// FontClass와 sentence 정보로 vertex array 생성
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// vertex array 정보를 sentence vertex buffer에 복사한다.
	// vertex buffer를 잠궈서 내용을 쓸 수 있게 한다.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// vertex buffer의 data 로의 포인터를 얻는다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// vertex buffer 에 데이터를 복사히 넣는다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// vertex buffer 잠금 해제
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// vertex array 해제
	delete[] vertices;
	vertices = 0;

	return true;

}

// sentence 자신과 vertex, index buffer를 해제
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

// RenderSentence 함수는 vertex와 index buffer를 input assembler에 넣고,
// FontShaderClass object에서 주어진 Sentence를 렌더하도록 한다.
// 현재의 view matrix가 아닌, m_baseMatrix를 쓰는 것에 주의하자. 이렇게 하면 현재의 view 가 매 프레임마다 변하더라도,
// 스크린상의 같은 위치에 텍스트를 그릴 수 있다. 
// 또한, 2d 공간에 그려져야 하기 때문에  projection matrix가 아닌 ortho Matrix를 사용한다.

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	XMVECTOR pixelColor;
	bool result;

	// vertec buffer의 stride와 offset를 지정한다.
	stride = sizeof(VertexType);
	offset = 0;

	// vertex buffer를 input assembler에서 활성 상태로 두어, 렌더할 수 있도록 한다.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// input sencence color로 pixel color vector를 만든다.
	pixelColor = XMVectorSet(sentence->red, sentence->blue, sentence->green, 1.0f);

	// font shader로 렌더한다.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);
	if (!result)
	{
		return false;
	}

	return true;

}

