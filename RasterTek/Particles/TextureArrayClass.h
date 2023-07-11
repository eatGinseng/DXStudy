////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTUREARRAYCLASS_H_
#define _TEXTUREARRAYCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11tex.h>
#include <d3dcompiler.h>

#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureArrayClass
{

	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*, char*, char*, HWND);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

private:
	bool LoadTarga(char*, int&, int&);

private:
	unsigned char* m_targaData;

	ID3D11Texture2D* m_texture[4];

	ID3D11ShaderResourceView* m_textureView[4];

};

#endif