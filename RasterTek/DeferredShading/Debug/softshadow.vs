cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer LightBuffer2
{
    float3 lightPosition;
    float padding;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 viewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};

PixelInputType SoftShadowVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;
    
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // soft shadow를 장면에 프로젝트 할 때의 projection 을 계산하기 위해 viewPosition을 사용한다.
    output.viewPosition = output.position;
    output.tex = input.tex;

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    worldPosition = mul(input.position, worldMatrix);

    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    output.lightPos = normalize(output.lightPos);

    return output;

}