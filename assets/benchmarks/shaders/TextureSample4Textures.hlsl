Texture2D    Tex0 : register(t0);
Texture2D    Tex1 : register(t1);
Texture2D    Tex2 : register(t2);
Texture2D    Tex3 : register(t3);
SamplerState Sampler0  : register(s4);

struct VSOutput {
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VSOutput vsmain(float4 Position : POSITION, float2 TexCoord : TEXCOORD0)
{
    VSOutput result;
    result.Position = Position;
    result.TexCoord = TexCoord;
    return result;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    return Tex0.Sample(Sampler0, input.TexCoord)
           + Tex1.Sample(Sampler0, input.TexCoord)
           + Tex2.Sample(Sampler0, input.TexCoord)
           + Tex3.Sample(Sampler0, input.TexCoord);
}
