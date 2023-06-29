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
    
    // position�� 4��° ��Ҹ� 1.0����
    input.position.w = 1.0f;
    
    // position ��ĺ�ȯ
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    
    // texture ��ǥ�� �����ؼ� �ȼ� ���̴��� �Ѱ��ش�.
    output.tex = input.tex;
    
    return output;
};

Texture2D shaderTexture;
SamplerState SampleType;

float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    
    // �ؽ�ó ��ǥ ��ġ�� �ȼ� �÷��� �ؽ�ó�κ��� ����. �̶�, sampler�� ���ȴ�.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
   // return float4(textureColor, 1.0f);
    return float4(1.0f, 0.0f, 0.0f, 1.0f);

}