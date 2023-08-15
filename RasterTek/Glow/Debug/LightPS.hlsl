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
    float3 color, textureColor;

    float lightIntensity;

    color = ambientColor;

    float3 LightDir = -lightDirection.xyz;

    lightIntensity = saturate(dot(input.normal, LightDir));
    float3 LightColor = diffuseColor.xyz;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex).xyz;
    
    if(lightIntensity > 0.0f)
    {
        // light intensity와 diffuse color로 최종 컬러를 결정
         color += (lightIntensity * LightColor);
         color = saturate(color);
    }

    color = color * textureColor;

    return float4(color, 1.0f);
}