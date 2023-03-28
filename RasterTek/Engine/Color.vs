
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
}

PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
	
	// matrix 계산을 위해 position vector를 4개짜리로 변환해 줌
	input.position.w = 1.0f;

	// position에 world, view, projection matrix 행렬을 곱해줌
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// pixel shader에서 쓸 수 있도록 Color를 넘겨줌
	output.color = input.color;

	return output;
}