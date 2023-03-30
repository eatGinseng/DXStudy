struct PixelInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

// don't forget to write ';' after creating struct, otherwise it will make an error.


float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{

	return input.color;
}