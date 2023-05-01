/////////////
// GLOBALS //

Texture2D shaderTexture[2];
SamplerState SampleType;

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 MultiTexturePixelShader(PixelInputType input) : SV_TARGET
{
    float4 color1;
    float4 color2;
    float4 blendColor;

    // first texture color
    color1 = shaderTexture[0].Sample(SampleType, input.tex);

    // second texture color
    color2 = shaderTexture[1].Sample(SampleType, input.tex);

    // blend pixel colors and multiply gamma valye
    blendColor = color1 * color2.r * 2.0;

    // Saturate the final color
    blendColor = saturate(blendColor);

    return blendColor;
}