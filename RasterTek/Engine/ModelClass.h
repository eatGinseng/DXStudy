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
		// shader의 VertexInputType의 구조와 매치되어야 함
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

	// 모델을 렌더링할 때 사용한 텍스처를 로드, 해제하는 함수
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:

	// vertex buffer와 index buffer 의 포인터.
	// DirectX 11 버퍼들은 모두 ID3D11Buffer 타입이며, 생성될 때 buffer description 에 의해 더 명확히 정의됨.
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;

	// 각 버퍼의 사이즈를 추적하기 위한 integer
	int m_vertexCount, m_indexCount;

	TextureClass* m_texture;
};

#endif
