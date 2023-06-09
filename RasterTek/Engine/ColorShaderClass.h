#pragma once
#pragma comment(lib, "d3dcompiler.lib")

#ifndef _COLORSHADERCLASS_H_
#define _COLORSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx11async.h>
#include <DirectXMath.h>
#include <fstream>
#include <d3dcompiler.h>

using namespace std;
using namespace DirectX;

class ColorShaderClass
{
private:
	// shader의 cbuffer와 일치해야함
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&);
	~ColorShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCWSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);
	
private:
	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

};


#endif