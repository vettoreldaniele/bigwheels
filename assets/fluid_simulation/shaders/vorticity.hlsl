#include "config.hlsli"

// FIXME(dnovillo) - Sample from these textures instead of InputTexture.
#if 0
Texture2D uVelocity : register(t0);
Texture2D uCurl : register(t1);
#endif

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    // FIXME(dnovillo) - Sample from uCurl
    float L = InputTexture.SampleLevel(Sampler, coord.uvL, 0).x;
    float R = InputTexture.SampleLevel(Sampler, coord.uvR, 0).x;
    float T = InputTexture.SampleLevel(Sampler, coord.uvT, 0).x;
    float B = InputTexture.SampleLevel(Sampler, coord.uvB, 0).x;
    float C = InputTexture.SampleLevel(Sampler, coord.uv, 0).x;

    float2 force = 0.5 * float2(abs(T) - abs(B), abs(R) - abs(L));
    force /= length(force) + 0.0001;
    force *= Params.curl * C;
    force.y *= -1.0;

    // FIXME(dnovillo) - Sample from uVelocity.
    float2 velocity = InputTexture.SampleLevel(Sampler, coord.uv, 0).xy;
    velocity += force * Params.dt;
    velocity = min(max(velocity, -1000.0), 1000.0);

    OutputTexture[coord.xy] = float4(velocity, 0.0, 1.0);
}
