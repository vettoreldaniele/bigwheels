
struct VSOutput {
	float4 Position : SV_POSITION;
	float3 Color    : COLOR;
};

VSOutput vsmain(float4 Position : POSITION, float3 Color : COLOR)
{
	VSOutput result;
	result.Position = Position;
	result.Color = Color;
	return result;
}

float4 psmain(VSOutput input) : SV_TARGET
{
	return float4(input.Color, 1);
}