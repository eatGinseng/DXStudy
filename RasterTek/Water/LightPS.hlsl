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
    float4 specular;
    float lightIntensity;

    // initialize the specular Color
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

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

         // reflection vector 계산
         reflection = normalize(2 * lightIntensity * input.normal - LightDir);

         // specular의 양은 reflection 벡터와 View Direction 벡터의 간격이 좁을수록 강해진다.
         specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

    }


    color = color * textureColor;

    // 마지막으로 specular color를 add 한다.
    color = saturate(color + specular);

    return float4(color, 1.0f);
}