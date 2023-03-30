#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <d3d11.h>
#include <D3DX11Tex.h>

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, LPCWSTR);
	void Shutdown();

	// texture로의 포인터를 반환한다. 이것을 셰이더에서 사용할 수 있다.
	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;

};

#endif