cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;

    // 텍스처를 project 할 viewpoint의 view, projection matrix
    matrix viewMatrix2;
    matrix projectionMatrix2;
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
};

PixelInputType ProjectionVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the position of the vertice as viewed by the projection view point in a separate variable.
    // 프로젝션할 view point 기준의 position을 별도의 variable에 저장
    output.viewPosition = mul(input.position, worldMatrix);
    output.viewPosition = mul(output.viewPosition, viewMatrix2);
    output.viewPosition = mul(output.viewPosition, projectionMatrix2);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    return output;
}
