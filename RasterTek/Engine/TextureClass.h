#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTK.lib")

#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include "stdafx.h"
#include <d3d11.h>
#include <D3DX11tex.h>

// #include <D3DX11async.h>
// #include <d3dcompiler.h>

class TextureClass
{

	
	
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	// texture로의 포인터를 반환한다. 이것을 셰이더에서 사용할 수 있다.
	ID3D11ShaderResourceView* GetTexture();

private:

	ID3D11Resource* m_textureResource;
	ID3D11Texture2D* m_texture2D;
	ID3D11ShaderResourceView* m_textureSRV;

};

#endif