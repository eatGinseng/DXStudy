#pragma once

#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "stdafx.h"
#include "TextureClass.h"

using namespace DirectX;


class ModelClass
{

private:
	struct VertexType
	{
		// shader�� VertexInputType�� ������ ��ġ�Ǿ�� ��
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// ���� �������� �� ����� �ؽ�ó�� �ε�, �����ϴ� �Լ�
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:

	// vertex buffer�� index buffer �� ������.
	// DirectX 11 ���۵��� ��� ID3D11Buffer Ÿ���̸�, ������ �� buffer description �� ���� �� ��Ȯ�� ���ǵ�.
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;

	// �� ������ ����� �����ϱ� ���� integer
	int m_vertexCount, m_indexCount;

	TextureClass* m_texture;
};

#endif
