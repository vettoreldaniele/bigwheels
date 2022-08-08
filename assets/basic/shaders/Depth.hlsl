
struct TransformData
{
    float4x4 ModelViewProjectionMatrix;
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
};

VSOutput vsmain(float4 Position
                : POSITION)
{
    VSOutput result;
    result.Position = mul(Transform.ModelViewProjectionMatrix, Position);
    return result;
}
