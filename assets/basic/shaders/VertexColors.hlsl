
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

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

VSOutput vsmain(float4 Position
                : POSITION, float3 Color
                : COLOR)
{
    VSOutput result;
    result.Position = mul(Transform.M, Position);
    result.Color    = Color;
    return result;
}

float4 psmain(VSOutput input)
    : SV_TARGET
{
    return float4(input.Color, 1);
}