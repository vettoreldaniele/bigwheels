#include "config.hlsli"

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord  coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);
    float4 sum   = 0.0;
    sum += InputTexture.SampleLevel(Sampler, coord.uvL, 0);
    sum += InputTexture.SampleLevel(Sampler, coord.uvR, 0);
    sum += InputTexture.SampleLevel(Sampler, coord.uvT, 0);
    sum += InputTexture.SampleLevel(Sampler, coord.uvB, 0);
    sum *= 0.25;
    OutputTexture[coord.xy] = sum * Params.intensity;
}
