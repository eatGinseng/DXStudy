cbuffer TessellationBuffer
{
    float tessellationAmount;
    float3 padding;
};
// this data will be set in ColorShderClass

// same as vertex output
struct HullInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

// patch constant function 으로부터 나올 output
struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct HullOutputType
{
    float3 position : POSITION;
    float4 color : COLOR;
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

