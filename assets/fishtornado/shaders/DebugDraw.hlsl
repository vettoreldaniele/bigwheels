#include "Config.hlsli"

#if defined(PPX_D3D11) // -----------------------------------------------------
cbuffer Scene : register(RENDER_SCENE_DATA_REGISTER)
{
    SceneData Scene;
};

cbuffer Model : register(RENDER_MODEL_DATA_REGISTER)
{
    ModelData Model;
};
#else // --- D3D12 / Vulkan ----------------------------------------------------
ConstantBuffer<SceneData> Scene : register(RENDER_SCENE_DATA_REGISTER, SCENE_SPACE);
ConstantBuffer<ModelData> Model : register(RENDER_MODEL_DATA_REGISTER, MODEL_SPACE);
#endif // -- defined (PPX_D3D11) -----------------------------------------------

VSOutput vsmain(float4 position : POSITION, float3 color : COLOR)
{
    float4x4 mvp = mul(Scene.viewProjectionMatrix, Model.modelMatrix);    
    
    VSOutput result   = (VSOutput)0;
    result.position   = mul(mvp, position);
    result.color      = color;
    
    return result;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    return float4(input.color, 1);
}
