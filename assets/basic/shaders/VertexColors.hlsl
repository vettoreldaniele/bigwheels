
struct TransformData
{
    float4x4 M;
};

ConstantBuffer<TransformData> Transform : register(b0);

struct VSOutput {
	float4 Position : SV_POSITION;
	float3 Color    : COLOR;
};

VSOutput vsmain(float4 Position : POSITION, float3 Color : COLOR)
{
	VSOutput result;
	result.Position = mul(Transform.M, Position);
	result.Color = Color;
	return result;
}

float4 psmain(VSOutput input) : SV_TARGET
{
	return float4(input.Color, 1);
}