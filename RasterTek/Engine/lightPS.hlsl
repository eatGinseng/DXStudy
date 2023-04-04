/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

// pixel shader light buffer
cbuffer LightBuffer
{
    float4 diffuseColor;
    float4 lightDirection;
    float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float3 textureColor;

    float3 LightDir = -lightDirection.xyz;
    float3 LightColor = diffuseColor.xyz;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    textureColor = textureColor * saturate(dot(input.normal, LightDir)) * LightColor;

    return float4(textureColor, 1.0f);
}