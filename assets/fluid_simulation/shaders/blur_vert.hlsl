struct VSInput
{
    float2 aPosition : POSITION;
};

struct PSInput
{
    float2 vUv : VUV;
    float2 vL : VL;
    float2 vR : VR;
    float4 pos : POSITION;
};

cbuffer texelSize : register(b0)
{
    float2 texelSize;
};

PSInput vsmain(VSInput input)
{
    PSInput output;
    output.vUv   = input.aPosition * 0.5 + 0.5;
    float offset = 1.33333333;
    output.vL    = output.vUv - texelSize * offset;
    output.vR    = output.vUv + texelSize * offset;
    output.pos   = float4(input.aPosition, 0.0, 1.0);
    return output;
}
