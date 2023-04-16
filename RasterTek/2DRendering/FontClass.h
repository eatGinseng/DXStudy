#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

#include <D3D11.h>
#include <DirectXMath.h>
#include <fstream>
using namespace DirectX;
using namespace std;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

class FontClass
{
private:
	// indexing data read from font index file.
	// left and right : texture uv coordinates
	// size는 문자의 픽셀 너비를 의미
	struct FontType
	{
		float left, right;
		int size;
	};

	// position과 texture uv 데이터만 갖고 있을 것임. 
	struct VertexType
	{
		XMVECTOR position;
		XMFLOAT2 texture;
	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*, char*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	
	// TextClass에서 호출할 것임, 인풋으로는 문장을 받고, 아웃풋으로는 문장을 표시할 트라이앵글의 vertex array를 돌려줌
	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

private:
	FontType* m_font;
	TextureClass* m_Texture;


};

#endif