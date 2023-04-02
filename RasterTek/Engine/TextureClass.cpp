#include "TextureClass.h"

TextureClass::TextureClass()
{
	m_textureResource = 0;
	m_texture2D = 0;
	m_textureSRV = 0;
}

TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{

	HRESULT result;

	// Device를 사용해서 파일로부터 texture를 shader resource 변수로 읽어들인다.
	
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_textureSRV, NULL);
	if (FAILED(result))
	{
		return false;
	}
	
	/* 여기서부터는 ChatGPT 발 Solution 
	result = D3DX11CreateTextureFromFile(
		device,  // Direct3D device
		filename,  // File path to the texture file
		nullptr,  // Texture info (nullptr for default)
		nullptr,  // Shader resource view description (nullptr for default)
		&m_textureResource,  // Pointer to the texture object
		nullptr); // Texture load result (nullptr for default)

	if (FAILED(result))
	{
		return false;
	}

	// Query the texture resource object for an ID3D11Texture2D interface.
	result = m_textureResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_texture2D));
	if (FAILED(result))
	{
		return false;
	}

	// Create the shader resource view for the texture2D object.
	// get texture2D description
	D3D11_TEXTURE2D_DESC textureDesc;
	m_texture2D->GetDesc(&textureDesc);

	// create shader resource view from texture2D and the mipmap information from tex2D description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	result = device->CreateShaderResourceView(m_texture2D, &srvDesc, &m_textureSRV);
	if (FAILED(result))
	{
		return false;
	}	
*/


	return true;

}

void TextureClass::Shutdown()
{
	if (m_textureResource)
	{
		m_textureResource->Release();
		m_textureResource = 0;
	}

	if (m_texture2D)
	{
		m_texture2D->Release();
		m_texture2D = 0;
	}

	// Texture resourceview 해제
	if (m_textureSRV)
	{
		m_textureSRV->Release();
		m_textureSRV = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureSRV;
}