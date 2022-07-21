#include "config.hlsli"

// FIXME(dnovillo) - This should sample from uVelocity.
#if 0
Texture2D uVelocity : register(t0);
#endif

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    // FIXME(dnovillo) - Sample from uVelocity.
    float L = InputTexture.SampleLevel(Sampler, coord.uvL, 0).x;
    float R = InputTexture.SampleLevel(Sampler, coord.uvR, 0).x;
    float T = InputTexture.SampleLevel(Sampler, coord.uvT, 0).y;
    float B = InputTexture.SampleLevel(Sampler, coord.uvB, 0).y;

    float2 C = InputTexture.SampleLevel(Sampler, coord.uv, 0).xy;
    if (coord.uvL.x < 0.0) {
        L = -C.x;
    }
    if (coord.uvR.x > 1.0) {
        R = -C.x;
    }
    if (coord.uvT.y > 1.0) {
        T = -C.y;
    }
    if (coord.uvB.y < 0.0) {
        B = -C.y;
    }

    float div               = 0.5 * (R - L + T - B);
    OutputTexture[coord.xy] = float4(div, 0.0, 0.0, 1.0);
}
