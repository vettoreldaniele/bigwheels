#include "Config.hlsli"

#if defined(PPX_D3D11) // -----------------------------------------------------
cbuffer Flocking : register(RENDER_FLOCKING_DATA_REGISTER)
{
    FlockingData Flocking;
};
Texture2D<float4>   InPositionTexture : register(RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER); // Previous position
Texture2D<float4>   InVelocityTexture : register(RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER);  // Current velocity
RWTexture2D<float4> OutPositionTexture : register(RENDER_OUTPUT_POSITION_TEXTURE_REGISTER);  // Out position
#else                                                                                        // --- D3D12 / Vulkan ----------------------------------------------------
ConstantBuffer<FlockingData> Flocking : register(RENDER_FLOCKING_DATA_REGISTER, space0);                      // Flocking params
Texture2D<float4>            InPositionTexture : register(RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, space0); // Previous position
Texture2D<float4>            InVelocityTexture : register(RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER, space0);  // Current velocity
RWTexture2D<float4>          OutPositionTexture : register(RENDER_OUTPUT_POSITION_TEXTURE_REGISTER, space0);  // Out position
#endif                                                                                       // -- defined (PPX_D3D11) -----------------------------------------------

// -------------------------------------------------------------------------------------------------

[numthreads(8, 8, 1)] void csmain(uint3 tid
                                  : SV_DispatchThreadID) {
#if 0
    OutPositionTexture[tid.xy] = InPositionTexture[tid.xy];
#else
    float4 Pos = InPositionTexture[tid.xy];
    float4 Vel = InVelocityTexture[tid.xy];

    float3 newPos = Pos.xyz + ((Vel.xyz) * (Vel.a * 0.1) * Flocking.timeDelta);

    OutPositionTexture[tid.xy] = float4(newPos, Pos.a);
#endif
}
