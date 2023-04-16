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
	// size�� ������ �ȼ� �ʺ� �ǹ�
	struct FontType
	{
		float left, right;
		int size;
	};

	// position�� texture uv �����͸� ���� ���� ����. 
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
	
	// TextClass���� ȣ���� ����, ��ǲ���δ� ������ �ް�, �ƿ�ǲ���δ� ������ ǥ���� Ʈ���̾ޱ��� vertex array�� ������
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