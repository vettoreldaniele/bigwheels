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
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
