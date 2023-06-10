const int NUM_LIGHTS = 4;

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;
using namespace DirectX;

class PointLightShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	// Diffuse COlor and Position for Light array
	struct LightColorBufferType
	{
		XMVECTOR diffuseColor[NUM_LIGHTS];
	};

	struct LightPositionBufferType
	{
		XMVECTOR lightPosition[NUM_LIGHTS];
	};

public:
	PointLightShaderClass();
	PointLightShaderClass(const PointLightShaderClass&);
	~PointLightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMVECTOR[], XMVECTOR[]);

private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCWSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMVECTOR[],
		XMVECTOR[]);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;

	ID3D11Buffer* m_lightColorBuffer;
	ID3D11Buffer* m_lightPositionBuffer;
};

