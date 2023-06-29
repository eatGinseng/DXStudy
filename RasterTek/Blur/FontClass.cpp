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

// Font Data하고 Font Texture 로드
bool FontClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR fontFilename, char* textureFilename, HWND hwnd)
{
	bool result;

	// font data txt 파일 로드
	result = LoadFontData(fontFilename);
	if(!result)
	{
		return false;
	}

	// 문자가 있는 텍스처
	result = LoadTexture(device, deviceContext, textureFilename, hwnd);
	if(!result)
	{
		return false;
	}

	return true;

}

void FontClass::Shutdown()
{
	// font 텍스처 release
	ReleaseTexture();

	// font data release
	ReleaseFontData();
}

bool FontClass::LoadFontData(LPCWSTR Filename)
{
	ifstream fin;
	int i;
	char temp;

	// FontType Structure 배열을 만든다. 배열의 길이는 95이다 : 데이터에 들어있는 문자의 총 갯수이다. 따라서 text파일의 인덱스 갯수도 95이다.
	// font spacing buffer
	m_font = new FontType[95];
	if (!m_font)
	{
		return false;
	}

	// Read in the font size and spacing between chars.
	fin.open(Filename);
	if (fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
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

	// Close the file.
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

// font dds file 을 shade resource로 읽어들인다. 
bool FontClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* Filename, HWND hwnd)
{
	bool result;

	m_Texture = new TextureClass();
	if (!m_Texture)
	{
		return false;
	}



	result = m_Texture->Initialize(device, deviceContext, Filename, hwnd);
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

// 이 함수는 TextClass로부터 호출될 것임. 인풋으로 문장을 받으면 그로부터 Vertex Buffer를 생성할 것임. 
// 각 문장은 저마다의 vertex buffer를 가지며, 이것으로 쉽게 렌더링 될 수있다. 
// 인풋 vertices 은 textureClass로 반환될 vertex array 로의 포인터이다.
// 인풋 sentence 는 vertex array를 만들 문장 자체이다. drawX, drawY는 문장을 그린 스크린상의 위치이다.
void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;

	// 아래 Loop는 Sentence의 각 문자를 받아 2개의 트라이앵글을 만들어 준다.
	// 그리고 나서 텍스처와 트라이앵글을 해당 문자로 맵핑해 준다 - m_FontArray로부터 받은 TU 텍스처 좌표와 Pixel size에 기반한다.
	// 해당 문자를 위한 폴리곤이 만들어지고 나면, 다음 문자를 그릴 스크린상의 X 좌표를 업데이트한다.

	// 각 문자를 quad 위에 그린다.
	for (i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(m_font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_font[letter].size + 1.0f;
		}
	}
	return;
}