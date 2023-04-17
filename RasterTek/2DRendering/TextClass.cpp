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
	result = m_Font->Initialize(device, L"../Engine/data/fontdata.txt", L"../Engine/data/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// 첫 번째 문장 초기화
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	// 문장의 Vertex Buffer를 새로운 스트링 정보로 채운다.
	result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
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

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext);
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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->vertexCount;
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

	// 

}