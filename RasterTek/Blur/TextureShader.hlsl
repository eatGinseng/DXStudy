cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex      : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
};



PixelInputType TextureVertexShader(VertexInputType input)
{
    
    PixelInputType output;
    
    // position의 4번째 요소를 1.0으로
    input.position.w = 1.0f;
    
    // position 행렬변환
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    
    // texture 좌표를 복사해서 픽셀 셰이더에 넘겨준다.
    output.tex = input.tex;
    
    return output;
};

Texture2D shaderTexture;
SamplerState SampleType;

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    
    // 텍스처 좌표 위치의 픽셀 컬러를 텍스처로부터 얻어낸다. 이때, sampler가 사용된다.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
   // return float4(textureColor, 1.0f);
    return float4(1.0f, 0.0f, 0.0f, 1.0f);

}