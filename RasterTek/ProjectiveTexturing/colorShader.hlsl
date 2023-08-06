struct VertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};


cbuffer TessellationBuffer
{
    float tessellationAmount;
    float3 padding;
};

// Hull shader로 전달할 데이터

struct HullInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct HullOutputType
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



// patch constant function 으로부터 나올 output
struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

// Hull shader는 각 output controll point에 대해 호출된다.
// patch constant function도 호출한다.
// hyll shader는 patch, control point의 in, patch id를 인풋으로 받는다.
// 여기에서는 color와 control point만 domain shader에 전달한다.
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;


    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = tessellationAmount;
    output.edges[1] = tessellationAmount;
    output.edges[2] = tessellationAmount;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside = tessellationAmount;

    return output;
}

// Hyll shader

[domain("tri")] // patch type : triangle
[partitioning("integer")] // tessellation partitioning type : integers
[outputtopology("triangle_cw")] // tessellator로부터의 output primitive type  : triangle
[outputcontrolpoints(3)] // number of control points that we will be outputting.
[patchconstantfunc("ColorPatchConstantFunction")] // constant function name

HullOutputType ColorHullShader(InputPatch<HullInputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HullOutputType output;


    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input color as the output color.
    output.color = patch[pointId].color;

    return output;
}

// domain shader : manipulate and tranform final vertices.
[domain("tri")]

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// triangle patch 인 1개의 attribute를 받는다.
// hull shader와 constant function 의 output을 input으로 받는다.

PixelInputType ColorDomainShader(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<HullOutputType, 3> patch)
{
    float3 vertexPosition;
    PixelInputType output;
 

    // Determine the position of the new vertex.
    vertexPosition = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Send the input color into the pixel shader.
    output.color = patch[0].color;

    return output;
}


float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
    return input.color;
}