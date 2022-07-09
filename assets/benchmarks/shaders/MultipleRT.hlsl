struct VSOutput {
    float4 Position : SV_POSITION;
};

VSOutput vsmain(float4 Position : POSITION)
{
    VSOutput result;
    result.Position = Position;
    return result;
}

struct PSOutput
{
    float4 rt0 : SV_TARGET0;
    float4 rt1 : SV_TARGET1;
    float4 rt2 : SV_TARGET2;
    float4 rt3 : SV_TARGET3;
};

PSOutput psmain(VSOutput input)
{
    PSOutput output;

    output.rt0 = float4(1.0f, 0.0f, 0.0f, 1.0f);
    output.rt1 = float4(0.0f, 1.0f, 0.0f, 1.0f);
    output.rt2 = float4(0.0f, 0.0f, 1.0f, 1.0f);
    output.rt3 = float4(1.0f, 1.0f, 0.0f, 1.0f);

    return output;
}
