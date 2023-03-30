#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_texture = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device, LPWSTR textureFilename)
{
	bool result;

	// 트라이앵글을 그리기 위한 지오메트리의 vertex buffer과 index buffer 초기화
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// model을 위한 텍스처를 로드한다.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;

}


void ModelClass::Shutdown()
{

	// model 텍스처를 release한다.
	ReleaseTexture();

	// vertex buffer와 index buffer 를 release
	ShutdownBuffers();

	return;
}

// vertex buffer과 index buffer를 렌더 파이프라인에 넣어주기 위해 RenderBuffers() 를 호출.
// 이렇게 하면 셰이더가 지오메트리를 렌더링 할 수 있음.
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

}

// color shader에서 인덱스 갯수가 필요할 것임. 그때 쓰는 함수
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

// vertex buffer와 index buffer를 생성하는 부분. 보통은 모델을 파일로부터 읽어들이지만 여기에서는 직접 set
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// 먼저 두 개의 임시 배열을 만든다. 이것을 final buffers로 채울 것이다.

	// vertex 갯수대로 채움
	m_vertexCount = 4;
	m_indexCount = 6;

	// vertex array 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// index array 생성
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}
	
	// 이제 배열에 버텍스와 인덱스 배열을 3개의 포인트로 채운다. 순서는 시계방향이어야 앞면으로 제대로 표시된다.
	// color도 vertex description의 일부이므로, green으로 넣어주었다.

	/* 삼각형의 경우
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // 좌측 하단
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // 중앙 위
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // 오른쪽 하단
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// data에서 index 배열을 받는다.
	indices[0] = 0; // 좌측 하단
	indices[1] = 1; // 중앙 상단
	indices[2] = 2; // 우측 하단
	*/

	// 사각형으로 바꾸기
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // 좌측 하단
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(-0.5f, 1.0f, 0.0f); // 좌측 상단
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // 오른쪽 하단
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].position = XMFLOAT3(0.5f, 1.0f, 0.0f); // 오른쪽 상단
	vertices[3].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// data에서 index 배열을 받는다.
	indices[0] = 0; // 좌측 하단
	indices[1] = 1; // 좌측 상단
	indices[2] = 2; // 우측 하단

	indices[3] = 2; // 좌측 하단
	indices[4] = 1; // 좌측 상단
	indices[5] = 3; // 우측 상단

	// 이제 배열을 만들었으니, 이걸로 버텍스 버퍼와 인덱스 버퍼를 만들 수 있다.
	// 두 버퍼는 같은 방식으로 만들어진다 - buffer의 description을 채운다.
	// decription에는 버퍼 사이즈(ByteWidth), 버퍼 타입(BindFlags)가 반드시 알맞게 들어가야 한다.
	// 그 이후에는 각각의 버퍼를 가리키는 Subresource pointer를 채워야 한다.
	// 이렇게 만든 description과 subresource pointer로 CreateBuffer()를 호출하면, 생성된 버퍼의 포인터를 반환한다.

	// static vertex buffer의 description을 만든다.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 버텍스 데이터로의 포인터를 Subresource structure에 제공한다.(?)
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 버텍스 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// 같은 방법으로 인덱스 버퍼도 생성해줌
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 앞에서 만든 vertex, index 배열은 필요없으니까 해제해 준다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

// Shutdown 함수는 initializeBuffers 에서 만든 버텍스 버퍼와 인덱스 버퍼를 해제한다.
void ModelClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}

// renderBuffers() 함수는 render() 함수에서 호출된다.
// 이 함수의 목적은 vertex buffer와 index buffer를 GPU의 Input Assembler가 활성화 되도록 하는 것이다.
// 일단 GPU에서 활성화 된 vertex buffer가 있으면, 셰이더를 사용해서 그 버퍼를 렌더링 할 수 있다.
// 또한 이 함수에서는 각 버퍼가 어떤 방식으로 그려져야 하는지 - 삼각형인지, 선인지 등을 결정한다.
// 여기에서는 버텍스 버퍼와 인덱스 버퍼를 Input Assembler에서 활성화하고, IASetPrimitiveeTopology() 로 트라이앵글로
// 그려지게 할 것이다.
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// vertex buffer의 stride와 offset 설정
	stride = sizeof(VertexType);
	offset = 0;

	// set vertex buffer active
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// set index buffer active
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 프리미티브가 이 버텍스 버퍼를 기준으로 렌더링되어야 하며, 트라이앵글로 렌더링 하도록 지시
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}