Texture2D shaderTexture;
SamplerState SampleType;

Texture2D reflectionTexture;


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
};

float4 ReflectionPixelShader(PixelInputType input) : SV_TARGET
{

    float4 textureColor;
    float2 reflectionTexCoord;
    float4 reflectionColor;
    float4 color;

    textureColor = shaderTexture.Sample(SampleType, input.tex);

    // input reflection position을 적절한 texture coordinated로 변환한다.
    // W coordinate로 나누어 주고, tu, ty 를 -1 ~ 1에서 0 ~ 1 사이로 맵핑해 준다.

    reflectionTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectionTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

    reflectionColor = reflectionTexture.Sample(SampleType, reflectionTexCoord);

    // floor의 텍스처와 reflection 된 상을 블렌딩 해 준다. 두 텍스처를 0.15factor로 lerp 해 준다.
    color = lerp(textureColor, reflectionColor, 0.15f);

    return color;
    // return float4(0.0, 1.0, 1.0, 1.0);
}