
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
	
	// matrix ����� ���� position vector�� 4��¥���� ��ȯ�� ��
	input.position.w = 1.0f;

	// position�� world, view, projection matrix ����� ������
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// pixel shader���� �� �� �ֵ��� Color�� �Ѱ���
	output.color = input.color;

	return output;
}