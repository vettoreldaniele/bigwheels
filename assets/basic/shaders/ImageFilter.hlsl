struct ParamsData
{
    float2 texel_size;
    int    filter;
};
// ConstantBuffer was added in SM5.1 for D3D12
//
#if defined(PPX_D3D11)
cbuffer Param : register(b1)
{
    ParamsData Param;
};
#else
ConstantBuffer<ParamsData> Param : register(b1);
#endif

RWTexture2D<float4> Output : register(u0);
SamplerState        nearestSampler : register(s2);
Texture2D<float4>   srcTex : register(t3);

float3 blurFilter(float2 coordCenter, float2 d);
float3 sharpenFilter(float2 coordCenter, float2 d);
float3 noFilter(float2 coordCenter);
float3 desaturate(float2 coordCenter);

[numthreads(1, 1, 1)] void csmain(uint3 tid
                                  : SV_DispatchThreadID) {
    const float2 coordCenter = Param.texel_size * (tid.xy + 0.5);
    float2       d           = Param.texel_size;

    float3 pixel;
    switch (Param.filter) {
        case 1:
            pixel = blurFilter(coordCenter, d);
            break;

        case 2:
            pixel = sharpenFilter(coordCenter, d);
            break;

        case 3:
            pixel = desaturate(coordCenter);
            break;

        default:
            pixel = noFilter(coordCenter);
            break;
    }

    Output[tid.xy] = float4(pixel, 1);
}

float3 blurFilter(float2 coordCenter, float2 d)
{
    // clang-format off
    const float2 samplingDeltas[3][3] = {
        {{-d.x, -d.y}, {0.0, -d.y}, {d.x, -d.y}},
        {{-d.x, 0.0}, {0.0, 0.0}, {d.x, 0.0}},
        {{-d.x, d.y}, {0.0, d.y}, {d.x, d.y}}};
    
    const float k = 1.0 / 16.0;

    const float filter[3][3] = {
        {1 * k, 2 * k, 1 * k},
        {2 * k, 4 * k, 2 * k},
        {1 * k, 2 * k, 1 * k}
    };
    // clang-format on

    float3 color = float3(0.0, 0.0, 0.0);
    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 3; ++i) {
            float2 sampleCoords = coordCenter + samplingDeltas[j][i];
            color += filter[j][i] * srcTex.SampleLevel(nearestSampler, sampleCoords, 0).rgb;
        }
    }

    return color;
}

float3 sharpenFilter(float2 coordCenter, float2 d)
{
    // clang-format off
    const float2 samplingDeltas[3][3] = {
        {{-d.x, -d.y}, {0.0, -d.y}, {d.x, -d.y}},
        {{-d.x, 0.0}, {0.0, 0.0}, {d.x, 0.0}},
        {{-d.x, d.y}, {0.0, d.y}, {d.x, d.y}}};
    
    const float k = 2.0;

    const float filter[3][3] = {
		{ -0.0625 * k, -0.0625 * k, -0.0625 * k},
		{ -0.0625 * k,     1.0 * k, -0.0625 * k},
		{ -0.0625 * k, -0.0625 * k, -0.0625 * k}
    };
    // clang-format on

    float3 color = float3(0.0, 0.0, 0.0);
    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 3; ++i) {
            float2 sampleCoords = coordCenter + samplingDeltas[j][i];
            color += filter[j][i] * srcTex.SampleLevel(nearestSampler, sampleCoords, 0).rgb;
        }
    }

    return color;
}

float3 desaturate(float2 coordCenter)
{
    float3 pixel     = srcTex.SampleLevel(nearestSampler, coordCenter, 0).rgb;
    float  grayLevel = pixel.r * 0.3 + pixel.g * 0.59 + pixel.b * 0.11;
    return float3(grayLevel, grayLevel, grayLevel);
}

float3 noFilter(float2 coordCenter)
{
    return srcTex.SampleLevel(nearestSampler, coordCenter, 0).rgb;
}
