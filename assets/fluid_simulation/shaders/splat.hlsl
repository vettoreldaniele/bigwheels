#include "config.hlsli"

// FIXME(dnovillo) - This should sample the uTarget texture.
#if 0
Texture2D uTarget : register(t0);
#endif

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    float2 p = coord.uv - Params.coordinate.xy;
    p.x *= Params.aspectRatio;
    float3 splat = exp(-dot(p, p) / Params.radius) * Params.color.rgb;
    // FIXME(dnovillo) - Sample from uTarget.
    float3 base = InputTexture.SampleLevel(Sampler, coord.uv, 0).xyz;

    OutputTexture[coord.xy] = float4(base + splat, 1.0);
}
