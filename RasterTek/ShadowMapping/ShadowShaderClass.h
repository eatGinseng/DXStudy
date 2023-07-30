#include <d3d11.h>
// #include <d3dx10math.h>
#include <DirectXMath.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;
using namespace DirectX;

class ShadowShaderClass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;

	};

	struct LightMatrixBufferType
	{
		XMMATRIX lightView;
		XMMATRIX lightProjection;

	};

	struct LightMatrixBufferType2
	{
		XMMATRIX lightView2;
		XMMATRIX lightProjection2;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT4 diffuseColor2;

	};

	struct LightBufferType2
	{
		XMFLOAT3 lightPosition;
		float padding1;
		XMFLOAT3 lightPosition2;
		float padding2;
	};

public:
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, XMFLOAT3, XMVECTOR, XMVECTOR, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMVECTOR );

private:
	bool InitializeShader(ID3D11Device*, HWND, LPCWSTR, LPCWSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCWSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4, XMFLOAT4, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4 );
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;

	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightMatrixBuffer;
	ID3D11Buffer* m_lightMatrixBuffer2;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightBuffer2;
};