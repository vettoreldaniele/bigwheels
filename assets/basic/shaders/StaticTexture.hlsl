
Texture2D    Tex0 : register(t0);
SamplerState Sampler0 : register(s1);

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
    result.Position = Position;
    result.TexCoord = TexCoord;
    return result;
}

float4 psmain(VSOutput input)
    : SV_TARGET
{
    return Tex0.Sample(Sampler0, input.TexCoord);
}