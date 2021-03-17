
#include "Config.hlsli"

#if defined (PPX_D3D11) // -----------------------------------------------------
cbuffer Scene : register(SCENE_CONSTANTS_REGISTER) 
{
    SceneData Scene;
};

cbuffer Material : register(MATERIAL_CONSTANTS_REGISTER) 
{
    MaterialData Material;
};

cbuffer Model : register(MODEL_CONSTANTS_REGISTER) 
{
    ModelData Model;
};

StructuredBuffer<Light> Lights : register(LIGHT_DATA_REGISTER);

Texture2D    AlbedoTex      : register(ALBEDO_TEXTURE_REGISTER);
Texture2D    RoughnessTex   : register(ROUGHNESS_TEXTURE_REGISTER);
Texture2D    MetalnessTex   : register(METALNESS_TEXTURE_REGISTER);
Texture2D    NormalMapTex   : register(NORMAL_MAP_TEXTURE_REGISTER);
SamplerState ClampedSampler : register(CLAMPED_SAMPLER_REGISTER);
#else // --- D3D12 / Vulkan ----------------------------------------------------
ConstantBuffer<SceneData>    Scene    : register(SCENE_CONSTANTS_REGISTER,    SCENE_DATA_SPACE);
ConstantBuffer<MaterialData> Material : register(MATERIAL_CONSTANTS_REGISTER, MATERIAL_DATA_SPACE);
ConstantBuffer<ModelData>    Model    : register(MODEL_CONSTANTS_REGISTER,    MODEL_DATA_SPACE);

StructuredBuffer<Light> Lights : register(LIGHT_DATA_REGISTER, SCENE_DATA_SPACE);
#endif // -- defined (PPX_D3D11) -----------------------------------------------

VSOutput vsmain(VSInput input)
{
    VSOutput output = (VSOutput)0;
    
    output.positionWS = mul(Model.modelMatrix, float4(input.position, 1)).xyz;
    output.position   = mul(Scene.viewProjectionMatrix, float4(output.positionWS, 1));
    output.normal     = mul(Model.normalMatrix, float4(input.normal, 0)).xyz;

    output.color    = input.color;
    output.texCoord = input.texCoord;
    
    // TBN
    output.normalTS    = mul(Model.modelMatrix, float4(input.normal, 0)).xyz;
    output.tangentTS   = mul(Model.modelMatrix, float4(input.tangent, 0)).xyz;
    output.bitangnetTS = mul(Model.modelMatrix, float4(input.bitangnet, 0)).xyz;    
    
    return output;
}