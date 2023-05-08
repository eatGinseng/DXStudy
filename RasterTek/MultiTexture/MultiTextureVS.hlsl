cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

cbuffer FogBuffer
{
    float fogStart;
    float fogEnd;
};

cbuffer ClipPlaneBuffer
{
    float4 clipPlane;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;

};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
    float fogFactor : FOG;
    float clip : SV_ClipDistance0;
};

PixelInputType MultiTextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;
    float4 CameraPosition;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Calculate the tangent vector against the world matrix only and then normalize the final value.
    //output.tangent = mul(input.tangent, (float3x3)worldMatrix);
    //output.tangent = normalize(output.tangent);

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    //output.binormal = mul(input.binormal, (float3x3)worldMatrix);
    // output.binormal = normalize(output.binormal);

    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);  

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);
    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    // calculate Z position of the vertex in view space.
    // use this data with the fog end and start position in the fog factor equation
    // and produce a fog factor, send it to pixel shader.
    CameraPosition = mul(input.position, worldMatrix);
    CameraPosition = mul(CameraPosition, viewMatrix);

    // calculate linear fog
    output.fogFactor = saturate((fogEnd - CameraPosition.z) / (fogEnd - fogStart));

    // Set the clipping plane.
    // vertex의 world space와 clip plane을 dot 한다. 이렇게 하면 pixel shader에서는 clip plane이 culling 하는 것은 그리지 않게 된다.
    output.clip = dot(mul(input.position, worldMatrix), clipPlane);

    return output;

}
