cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer ReflectionBuffer
{
    matrix reflectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;

};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
};

PixelInputType ReflectionVertexShader(VertexInputType input)
{
    PixelInputType output;
    matrix reflectProjectWorld;
	

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // input position 값들을 투영된 reflection position으로 변환한다.
    // 이 matrix는 reflection matrix, projection matrix, world matrix의 혼합이다.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

    output.reflectionPosition = mul(input.position, reflectProjectWorld);

    return output;
}