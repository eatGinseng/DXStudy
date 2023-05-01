#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

#include "fontclass.h"
#include "fontshaderclass.h"
#include <DirectXMath.h>
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	// 각 text sentence 의 렌더링 정보를 담고 있는 structure
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer, * indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	// FontClass의 Vertex Type과 일치해야 한다.
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);

	bool SetMousePosition(int, int, ID3D11DeviceContext*);

	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);
	bool SetRenderCount(int, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);

	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX, XMMATRIX);

private:
	FontClass* m_Font;
	FontShaderClass* m_FontShader;
	int m_screenWidth, m_screenHeight;
	XMMATRIX m_baseViewMatrix;

	// mouseX, mouseY
	SentenceType* m_sentence1;
	SentenceType* m_sentence2;

	// FPS, CPU usage
	SentenceType* m_sentence3;
	SentenceType* m_sentence4;

	// Model Count
	SentenceType* m_sentence5;

};

#endif