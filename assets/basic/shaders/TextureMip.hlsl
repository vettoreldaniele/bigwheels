

// ConstantBuffer was addd in SM5.1 for D3D12
//
#if defined(PPX_D3D11)
struct InputData
{
    float4x4 M;
    int      mipLevel;
};

cbuffer Constants : register(b0)
{
    InputData Constants;
};
#else
struct InputData
{
    float4x4 M;
    int      mipLevel;
};

ConstantBuffer<InputData> Constants : register(b0);
#endif

Texture2D    Tex0 : register(t1);
SamplerState Sampler0 : register(s2);

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VSOutput vsmain(float4 Position
                : POSITION, float2 TexCoord
                : TEXCOORD0)
{
    VSOutput result;
    result.Position = mul(Constants.M, Position);
    result.TexCoord = TexCoord;
    return result;
}

float4 psmain(VSOutput input)
    : SV_TARGET
{
    return Tex0.SampleLevel(Sampler0, input.TexCoord, Constants.mipLevel);
}