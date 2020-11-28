#include "Config.hlsli"

ConstantBuffer<FlockingData> Flocking           : register(b0, space0); // Flocking params
Texture2D<float4>            InPositionTexture  : register(t1, space0); // Previous position
Texture2D<float4>            InVelocityTexture  : register(t2, space0); // Current velocity
RWTexture2D<float4>          OutPositionTexture : register(u3, space0); // Out position

// -------------------------------------------------------------------------------------------------

[numthreads(8, 8, 1)] 
void csmain(uint3 tid : SV_DispatchThreadID) 
{
    float4 Pos = InPositionTexture[tid.xy];
    float4 Vel = InVelocityTexture[tid.xy];

    float3 newPos = Pos.xyz + ((Vel.xyz) * (Vel.a * 0.1) * Flocking.timeDelta);

    OutPositionTexture[tid.xy] = float4(newPos, Pos.a);
}
