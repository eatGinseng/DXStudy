///////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fontclass.h"

FontClass::FontClass()
{
	m_font = 0;
	m_Texture = 0;
}

FontClass::FontClass(const FontClass& other)
{

}

FontClass::~FontClass()
{

}

// Font Data�ϰ� Font Texture �ε�
bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR fontFilename, LPCWSTR textureFilename, HWND hwnd)
{
	bool result;

	// font data txt ���� �ε�
	result = LoadFontData(fontFilename);
	if(!result)
	{
		return false;
	}

	// ���ڰ� �ִ� �ؽ�ó
	result = LoadTexture(device, deviceContext, textureFilename, hwnd);
	if(!result)
	{
		return false;
	}

	return true;

}

void FontClass::Shutdown()
{
	// font �ؽ�ó release
	ReleaseTexture();

	// font data release
	ReleaseFontData();
}

bool FontClass::LoadFontData(LPCWSTR Filename)
{
	ifstream fin;
	int i;
	char temp;

	// FontType Structure �迭�� �����. �迭�� ���̴� 95�̴� : �����Ϳ� ����ִ� ������ �� �����̴�. ���� text������ �ε��� ������ 95�̴�.
	// font spacing buffer
	m_font = new FontType[95];
	if (!m_font)
	{
		return false;
	}

	fin.open(Filename);
	if (fin.fail())
	{
		return false;
	}

	for (i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_font[i].left;
		fin >> m_font[i].right;
		fin >> m_font[i].size;
	}

	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	if (m_font)
	{
		delete[] m_font;
		m_font = 0;
	}

	return;
}

// font dds file �� shade resource�� �о���δ�. 
bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR Filename, HWND hwnd)
{
	bool result;

	m_Texture = new TextureClass();
	if (!m_Texture)
	{
		return false;
	}

	// convert LPCWSTR filename to char*
	int len = wcslen(Filename);

	char* buffer = new char[len];

	WideCharToMultiByte(CP_ACP, 0, Filename, -1, buffer, len, NULL, NULL);

	std::string strFilename(buffer);
	std::cout << strFilename << std::endl;

	delete[] buffer;

	result = m_Texture->Initialize(device, deviceContext, strFilename, hwnd);
	if (!result)
	{
		return false;
	}

	return true;

}

void FontClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
	return;
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

// �� �Լ��� TextClass�κ��� ȣ��� ����. ��ǲ���� ������ ������ �׷κ��� Vertex Buffer�� ������ ����. 
// �� ������ �������� vertex buffer�� ������, �̰����� ���� ������ �� ���ִ�. 
// ��ǲ vertices �� textureClass�� ��ȯ�� vertex array ���� �������̴�.
// ��ǲ sentence �� vertex array�� ���� ���� ��ü�̴�. drawX, drawY�� ������ �׸� ��ũ������ ��ġ�̴�.
void FontClass::BuildVertexArray(void* vertices, const char sentence[128], float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;

	// vertices ��ǲ�� vertexType structure�� ������ �ִ´�.
	vertexPtr = (VertexType*)vertices;

	// sentents ���� ���� ���� ��´�.
	numLetters = (int)strlen(sentence);

	// vertex array�� index�� �ʱ�ȭ
	index = 0;

	// �Ʒ� Loop�� Sentence�� �� ���ڸ� �޾� 2���� Ʈ���̾ޱ��� ����� �ش�.
	// �׸��� ���� �ؽ�ó�� Ʈ���̾ޱ��� �ش� ���ڷ� ������ �ش� - m_FontArray�κ��� ���� TU �ؽ�ó ��ǥ�� Pixel size�� ����Ѵ�.
	// �ش� ���ڸ� ���� �������� ��������� ����, ���� ���ڸ� �׸� ��ũ������ X ��ǥ�� ������Ʈ�Ѵ�.

	// �� ���ڸ� quad ���� �׸���.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// ���� letter�� �����̶��, �׳� 3�ȼ� ������ �Űܶ�
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = XMVectorSet(drawX, drawY, 0.0f, 1.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMVectorSet((drawX + m_font[letter].size), (drawY - 16), 0.0f, 1.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMVectorSet(drawX, (drawY - 16), 0.0f, 1.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = XMVectorSet(drawX, drawY, 0.0f, 1.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMVectorSet(drawX + m_font[letter].size, drawY, 0.0f, 1.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMVectorSet((drawX + m_font[letter].size), (drawY - 16), 0.0f, 1.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_font[letter].size + 1.0f;
		}
	}
	return;
}