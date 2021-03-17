
struct TransformData
{
    float4x4 M;
};

// ConstantBuffer was addd in SM5.1 for D3D12
//
#if defined(PPX_D3D11)
cbuffer Transform : register(b0)
{
    TransformData Transform;
};
#else
ConstantBuffer<TransformData> Transform : register(b0);
#endif // defined(PPX_D3D11)

TextureCube                   Tex0      : register(t1);
SamplerState                  Sampler0  : register(s2);

struct VSOutput {
	float4 Position : SV_POSITION;
	float3 TexCoord : TEXCOORD;
};

VSOutput vsmain(float4 Position : POSITION)
{
	VSOutput result;
	result.Position = mul(Transform.M, Position);
	result.TexCoord = Position.xyz;
	return result;
}

float4 psmain(VSOutput input) : SV_TARGET
{
	return Tex0.Sample(Sampler0, input.TexCoord);
}