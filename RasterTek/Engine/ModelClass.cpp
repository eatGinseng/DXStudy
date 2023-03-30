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

	// Ʈ���̾ޱ��� �׸��� ���� ������Ʈ���� vertex buffer�� index buffer �ʱ�ȭ
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// model�� ���� �ؽ�ó�� �ε��Ѵ�.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;

}


void ModelClass::Shutdown()
{

	// model �ؽ�ó�� release�Ѵ�.
	ReleaseTexture();

	// vertex buffer�� index buffer �� release
	ShutdownBuffers();

	return;
}

// vertex buffer�� index buffer�� ���� ���������ο� �־��ֱ� ���� RenderBuffers() �� ȣ��.
// �̷��� �ϸ� ���̴��� ������Ʈ���� ������ �� �� ����.
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);

}

// color shader���� �ε��� ������ �ʿ��� ����. �׶� ���� �Լ�
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

// vertex buffer�� index buffer�� �����ϴ� �κ�. ������ ���� ���Ϸκ��� �о�������� ���⿡���� ���� set
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// ���� �� ���� �ӽ� �迭�� �����. �̰��� final buffers�� ä�� ���̴�.

	// vertex ������� ä��
	m_vertexCount = 4;
	m_indexCount = 6;

	// vertex array ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// index array ����
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}
	
	// ���� �迭�� ���ؽ��� �ε��� �迭�� 3���� ����Ʈ�� ä���. ������ �ð�����̾�� �ո����� ����� ǥ�õȴ�.
	// color�� vertex description�� �Ϻ��̹Ƿ�, green���� �־��־���.

	/* �ﰢ���� ���
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // ���� �ϴ�
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	
	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // �߾� ��
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	
	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // ������ �ϴ�
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// data���� index �迭�� �޴´�.
	indices[0] = 0; // ���� �ϴ�
	indices[1] = 1; // �߾� ���
	indices[2] = 2; // ���� �ϴ�
	*/

	// �簢������ �ٲٱ�
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // ���� �ϴ�
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(-0.5f, 1.0f, 0.0f); // ���� ���
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // ������ �ϴ�
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].position = XMFLOAT3(0.5f, 1.0f, 0.0f); // ������ ���
	vertices[3].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// data���� index �迭�� �޴´�.
	indices[0] = 0; // ���� �ϴ�
	indices[1] = 1; // ���� ���
	indices[2] = 2; // ���� �ϴ�

	indices[3] = 2; // ���� �ϴ�
	indices[4] = 1; // ���� ���
	indices[5] = 3; // ���� ���

	// ���� �迭�� ���������, �̰ɷ� ���ؽ� ���ۿ� �ε��� ���۸� ���� �� �ִ�.
	// �� ���۴� ���� ������� ��������� - buffer�� description�� ä���.
	// decription���� ���� ������(ByteWidth), ���� Ÿ��(BindFlags)�� �ݵ�� �˸°� ���� �Ѵ�.
	// �� ���Ŀ��� ������ ���۸� ����Ű�� Subresource pointer�� ä���� �Ѵ�.
	// �̷��� ���� description�� subresource pointer�� CreateBuffer()�� ȣ���ϸ�, ������ ������ �����͸� ��ȯ�Ѵ�.

	// static vertex buffer�� description�� �����.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���ؽ� �����ͷ��� �����͸� Subresource structure�� �����Ѵ�.(?)
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���ؽ� ���� ����
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// ���� ������� �ε��� ���۵� ��������
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

	// �տ��� ���� vertex, index �迭�� �ʿ�����ϱ� ������ �ش�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

// Shutdown �Լ��� initializeBuffers ���� ���� ���ؽ� ���ۿ� �ε��� ���۸� �����Ѵ�.
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

// renderBuffers() �Լ��� render() �Լ����� ȣ��ȴ�.
// �� �Լ��� ������ vertex buffer�� index buffer�� GPU�� Input Assembler�� Ȱ��ȭ �ǵ��� �ϴ� ���̴�.
// �ϴ� GPU���� Ȱ��ȭ �� vertex buffer�� ������, ���̴��� ����ؼ� �� ���۸� ������ �� �� �ִ�.
// ���� �� �Լ������� �� ���۰� � ������� �׷����� �ϴ��� - �ﰢ������, ������ ���� �����Ѵ�.
// ���⿡���� ���ؽ� ���ۿ� �ε��� ���۸� Input Assembler���� Ȱ��ȭ�ϰ�, IASetPrimitiveeTopology() �� Ʈ���̾ޱ۷�
// �׷����� �� ���̴�.
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// vertex buffer�� stride�� offset ����
	stride = sizeof(VertexType);
	offset = 0;

	// set vertex buffer active
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// set index buffer active
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ������Ƽ�갡 �� ���ؽ� ���۸� �������� �������Ǿ�� �ϸ�, Ʈ���̾ޱ۷� ������ �ϵ��� ����
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}