#include "config.hlsli"

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord coord             = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);
    OutputTexture[coord.xy] = Params.clearValue * InputTexture.SampleLevel(Sampler, coord.uv, 0);
}
