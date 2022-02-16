Texture2D Tex0 : register(t0);
Texture2D Tex1 : register(t1);
Texture2D Tex2 : register(t2);
Texture2D Tex3 : register(t3);

struct VSOutput {
    float4 Position : SV_POSITION;
};

VSOutput vsmain(float4 Position : POSITION)
{
    VSOutput result;
    result.Position = Position;
    return result;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    uint3 loadArgs = uint3(input.Position.x, input.Position.y, /* mipmap */ 0);
    return Tex0.Load(loadArgs) 
           + Tex1.Load(loadArgs)
           + Tex2.Load(loadArgs)
           + Tex3.Load(loadArgs);
}

