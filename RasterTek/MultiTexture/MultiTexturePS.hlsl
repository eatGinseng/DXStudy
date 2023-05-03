/////////////
// GLOBALS //

Texture2D shaderTexture[4];
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
}

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 MultiTexturePixelShader(PixelInputType input) : SV_TARGET
{
    float4 color1;
    float4 color2;
    float alpha;
    float4 bumpColor;
    float3 bumpNormal;

    float4 blendColor;

    float3 LightDir;
    float lightIntensity;


    // first texture color
    color1 = shaderTexture[0].Sample(SampleType, input.tex);

    // second texture color
    color2 = shaderTexture[1].Sample(SampleType, input.tex);

    // alpha
    alpha = shaderTexture[2].Sample(SampleType, input.tex).x;

    // normal
    bumpColor = shaderTexture[3].Sample(SampleType, input.tex);

    // Calculate the normal from the data in the bump map.
    bumpNormal = (bumpColor.x * input.tangent) + (bumpColor.y * input.binormal) + (bumpColor.z * input.normal);
    bumpNormal = normalize(bumpNormal);

    LightDir = -lightDirection;

    lightIntensity = saturate(dot(bumpNormal, LightDir));

    // blend pixel colors and multiply gamma valye
    blendColor = saturate(lightIntensity * diffuseColor * lerp(color1, color2, alpha));

    return blendColor;
}