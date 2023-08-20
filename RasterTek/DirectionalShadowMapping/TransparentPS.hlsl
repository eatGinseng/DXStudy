

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

cbuffer transparentBuffer
{
    float blendAmount;
    float4 padding;
};

Texture2D shaderTexture;
SamplerState SampleType;

float4 TransparentPixelShader(PixelInputType input) : SV_TARGET
{
    float4 color;
	
	
    // Sample the texture pixel at this location.
    color = shaderTexture.Sample(SampleType, input.tex);
    
    // 픽셀의 알파값을 결정하는 데 Blend amount를 사용
    // Set the alpha value of this pixel to the blending amount to create the alpha blending effect.
    color.a = blendAmount;

    return color;
}
