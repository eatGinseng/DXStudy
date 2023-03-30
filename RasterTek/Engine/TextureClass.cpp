#include "TextureClass.h"

TextureClass::TextureClass()
{
	m_texture = 0;
}

TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, LPCWSTR filename)
{

	HRESULT result;

	// Device를 사용해서 파일로부터 texture를 shader resource 변수로 읽어들인다.

	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;

}

void TextureClass::Shutdown()
{

	// Texture resource 해제
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}