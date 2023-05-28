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

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

// reflection coordinates, refraction coordinates.

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
};

PixelInputType WaterVertexShader(VertexInputType input)
{
    PixelInputType output;
    matrix reflectProjectWorld;
    matrix viewProjectWorld;
	
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Create the refraction projection world matrix. reflection matrix 만들었을 때 처럼.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);
    output.reflectionPosition = mul(input.position, reflectProjectWorld);

    // refraction을 위한 view projection world matrix 만들기
    viewProjectWorld = mul(viewMatrix, projectionMatrix);
    viewProjectWorld = mul(worldMatrix, viewProjectWorld);

    // viewProjectWorldMatrix에 대한 input position 생성
    output.refractionPosition = mul(input.position, viewProjectWorld);

    return output;

}