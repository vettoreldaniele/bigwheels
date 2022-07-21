#include "config.hlsli"

// FIXME(dnovillo) - Sample from uPressure and uDivergence.
#if 0
Texture2D uPressure : register(t0);
Texture2D uDivergence : register(t1);
#endif

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);

    // FIXME(dnovillo) - Sample from uPressure.
    float L = InputTexture.SampleLevel(Sampler, coord.uvL, 0).x;
    float R = InputTexture.SampleLevel(Sampler, coord.uvR, 0).x;
    float T = InputTexture.SampleLevel(Sampler, coord.uvT, 0).x;
    float B = InputTexture.SampleLevel(Sampler, coord.uvB, 0).x;
    float C = InputTexture.SampleLevel(Sampler, coord.uv, 0).x;

    // FIXME(dnovillo) - Sample from uDivergence.
    float divergence = InputTexture.SampleLevel(Sampler, coord.uv, 0).x;
    float pressure   = (L + R + B + T - divergence) * 0.25;

    OutputTexture[coord.xy] = float4(pressure, 0.0, 0.0, 1.0);
}
