#include "config.hlsli"

#define SCALE 25.0

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord  coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);
    float2 uv    = floor(coord.uv * SCALE * float2(Params.aspectRatio, 1.0));
    float  v     = fmod(uv.x + uv.y, 2.0);
    v            = v * 0.1 + 0.8;

    OutputTexture[coord.xy] = float4(float3(v, v, v), 1.0);
}
