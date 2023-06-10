
#ifndef _GLASSSHADERCLASS_H_
#define _GLASSSHADERCLASS_H_


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

////////////////////////////////////////////////////////////////////////////////
// Class name: GlassShaderClass
////////////////////////////////////////////////////////////////////////////////
class GlassShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	// refraction scale를 조정하기 위한 constant buffer
	struct GlassBufferType
	{
		float refractionScale;
		XMFLOAT3 padding;
	};

public:
	GlassShaderClass();
	GlassShaderClass(const GlassShaderClass&);
	~GlassShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*,
		int,
		XMMATRIX worldMatrix,
		XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* colorTexture,
		ID3D11ShaderResourceView* normalTexture,
		ID3D11ShaderResourceView* refractionTexture,
		float refractionScale);
private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCWSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_glassBuffer;
	ID3D11SamplerState* m_sampleState;

};

#endif