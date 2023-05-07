/////////////
// GLOBALS //

Texture2D shaderTexture[4];
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
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
    float3 viewDirection : TEXCOORD1;
    float fogFactor : FOG;

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

    float4 fogColor;

    float4 blendColor;

    float3 LightDir;
    float lightIntensity;

    float3 ViewDir = normalize(input.viewDirection);

    // first texture color
    color1 = shaderTexture[0].Sample(SampleType, input.tex);

    // second texture color
    color2 = shaderTexture[1].Sample(SampleType, input.tex);

    // alpha
    alpha = shaderTexture[2].Sample(SampleType, input.tex).x;

    // normal
    bumpColor = shaderTexture[3].Sample(SampleType, input.tex);
    bumpColor = (bumpColor * 2.0f) - 1.0f;

    // TBN
    float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);
    TBN = transpose(TBN);

    // tangent space normal
    bumpNormal = mul(bumpColor, TBN);


    // Calculate the normal from the data in the bump map.
    // bumpNormal = (bumpColor.x * input.tangent) + (bumpColor.y * input.binormal) + (bumpColor.z * input.normal);
    // bumpNormal = normalize(bumpNormal);

    LightDir = -lightDirection;
    float3 HalfVector = normalize(LightDir + ViewDir);
    float Specular = saturate(dot(bumpNormal, HalfVector));
    Specular = pow(Specular, 4.0);

    lightIntensity = saturate(dot(bumpNormal, LightDir));

    // blend pixel colors and multiply gamma valye
    blendColor = saturate(lightIntensity * diffuseColor * lerp(color1, color2, alpha));
    blendColor += Specular;

    // Set the color of the fog to grey.
    fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);

    // Calculate the final color using the fog effect equation.
    blendColor = input.fogFactor * blendColor + (1.0 - input.fogFactor) * fogColor;

    return blendColor;
}