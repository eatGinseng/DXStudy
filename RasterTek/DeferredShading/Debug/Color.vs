struct VertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

// Hull shader로 전달할 데이터

struct HullInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

HullInputType ColorVertexShader(VertexInputType input)
{
    HullInputType output;
    // Pass the vertex position into the hull shader.
    output.position = input.position;
    
    // Pass the input color into the hull shader.
    output.color = input.color;
    
    return output;
}
