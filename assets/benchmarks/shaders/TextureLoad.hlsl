Texture2D Tex0 : register(t0);

struct VSOutput
{
    float4 Position : SV_POSITION;
};

VSOutput vsmain(float4 Position
                : POSITION)
{
    VSOutput result;
    result.Position = Position;
    return result;
}

float4 psmain(VSOutput input)
    : SV_TARGET
{
    return Tex0.Load(uint3(input.Position.x, input.Position.y, /* mipmap */ 0));
}
