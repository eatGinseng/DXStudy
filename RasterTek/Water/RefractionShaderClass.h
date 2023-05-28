#pragma once
#ifndef _REFRACTIONSHADERCLASS_H_
#define _REFRACTIONSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <DirectXMath.h>
#include <fstream>
using namespace std;
using namespace DirectX;

class RefractionShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMVECTOR ambientColor;
		XMVECTOR diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

	struct ClipPlaneBufferType
	{
		XMVECTOR clipPlane;
	};

public:
	RefractionShaderClass();
	RefractionShaderClass(const RefractionShaderClass&);
	~RefractionShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMVECTOR,
		XMVECTOR, XMVECTOR);

private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCWSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3,
		XMVECTOR, XMVECTOR, XMVECTOR);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	ID3D11Buffer* m_clipPlaneBuffer;

};

#endif