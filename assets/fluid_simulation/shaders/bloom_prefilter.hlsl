#include "config.hlsli"

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord  coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);
    float3 c     = InputTexture.SampleLevel(Sampler, coord.uv, 0).rgb;
    float  br    = max(c.r, max(c.g, c.b));
    float  rq    = clamp(br - Params.curve.x, 0.0, Params.curve.y);
    rq           = Params.curve.z * rq * rq;
    c *= max(rq, br - Params.threshold) / max(br, 0.0001);
    OutputTexture[coord.xy] = float4(c, 0.0);
}
