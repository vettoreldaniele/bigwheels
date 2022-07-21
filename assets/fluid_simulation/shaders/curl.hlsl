#include "config.hlsli"

// FIXME(dnovillo) - This may need to use a different input texture.
#if 0
Texture2D uVelocity : register(t0);
#endif

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    // FIXME(dnovillo) - This will need to sample from uVelocity.
    float L                 = InputTexture.SampleLevel(Sampler, coord.uvL, 0).y;
    float R                 = InputTexture.SampleLevel(Sampler, coord.uvR, 0).y;
    float T                 = InputTexture.SampleLevel(Sampler, coord.uvT, 0).x;
    float B                 = InputTexture.SampleLevel(Sampler, coord.uvB, 0).x;
    float vorticity         = R - L - T + B;
    OutputTexture[coord.xy] = float4(0.5 * vorticity, 0.0, 0.0, 1.0);
}
