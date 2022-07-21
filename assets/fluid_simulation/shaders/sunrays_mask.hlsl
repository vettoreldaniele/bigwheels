#include "config.hlsli"

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    float4 c  = InputTexture.SampleLevel(Sampler, coord.uv, 0);
    float  br = max(c.r, max(c.g, c.b));
    c.a       = 1.0 - min(max(br * 20.0, 0.0), 0.8);

    OutputTexture[coord.xy] = c;
}
