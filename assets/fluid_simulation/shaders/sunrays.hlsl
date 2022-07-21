#include "config.hlsli"

#define ITERATIONS 16

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord inputCoord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    float Density  = 0.3;
    float Decay    = 0.95;
    float Exposure = 0.7;

    float2 coord = inputCoord.uv;
    float2 dir   = inputCoord.uv - (float2)0.5;

    dir *= 1.0 / float(ITERATIONS) * Density;
    float illuminationDecay = 1.0;

    float color = InputTexture.SampleLevel(Sampler, coord, 0).a;

    for (int i = 0; i < ITERATIONS; i++) {
        coord -= dir;
        float col = InputTexture.SampleLevel(Sampler, coord, 0).a;
        color += col * illuminationDecay * Params.weight;
        illuminationDecay *= Decay;
    }

    OutputTexture[inputCoord.xy] = float4(color * Exposure, 0.0, 0.0, 1.0);
}
