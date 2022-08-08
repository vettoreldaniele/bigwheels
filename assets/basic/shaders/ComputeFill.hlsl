
RWTexture2D<float4> Output : register(u0);

[numthreads(1, 1, 1)] void csmain(uint3 tid
                                  : SV_DispatchThreadID) {
    Output[tid.xy] = float4(1, 0, 0, 1);
}