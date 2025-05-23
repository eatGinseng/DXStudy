#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

#include <DirectXMath.h>
using namespace DirectX;
using namespace std;

class LightShaderClass {

private:

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMVECTOR lightDirection;
		float padding;
	};

	struct LightMatrixBufferType
	{
		XMMATRIX lightViewMatrix;
		XMMATRIX lightOrthoMatrix;
	};


public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMVECTOR, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCWSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, XMVECTOR, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightMatrixBuffer;

	ID3D11SamplerState* m_sampleState;
};


#endif