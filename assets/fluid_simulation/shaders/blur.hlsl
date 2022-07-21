#include "config.hlsli"

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord  coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);
    float4 sum   = InputTexture.SampleLevel(Sampler, coord.uv, 0) * 0.29411764;
    sum += InputTexture.SampleLevel(Sampler, coord.uvL, 0) * 0.35294117;
    sum += InputTexture.SampleLevel(Sampler, coord.uvR, 0) * 0.35294117;
    OutputTexture[coord.xy] = sum;
}
