/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

// pixel shader light buffer
cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 lightDirection;
    float specularPower;
    float4 specularColor;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float3 color, textureColor, reflection;
    float lightIntensity;

    float3 LightDir = -lightDirection.xyz;

    lightIntensity = saturate(dot(input.normal, LightDir));
    float3 LightColor = diffuseColor.xyz;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex).xyz;
    
    color = LightColor * lightIntensity * textureColor;


    return float4(color, 1.0f);
}