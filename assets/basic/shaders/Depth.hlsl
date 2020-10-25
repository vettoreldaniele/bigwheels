
struct TransformData
{
    float4x4 ModelViewProjectionMatrix;
};

ConstantBuffer<TransformData> Transform : register(b0);

struct VSOutput {
	float4 Position : SV_POSITION;
};

VSOutput vsmain(float4 Position : POSITION)
{
	VSOutput result;
	result.Position = mul(Transform.ModelViewProjectionMatrix, Position);
	return result;
}

