
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

Texture2D    AlbedoTex      : register(ALBEDO_TEXTURE_REGISTER,  MATERIAL_RESOURCES_SPACE);
SamplerState ClampedSampler : register(CLAMPED_SAMPLER_REGISTER, MATERIAL_RESOURCES_SPACE);
#endif // -- defined (PPX_D3D11) -----------------------------------------------


float Lambert(float3 N, float3 L)
{
    float diffuse = saturate(dot(N, L));
    return diffuse;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    float3 N = normalize(input.normal); 
    
    float3 albedo = Material.albedo;
    if (Material.albedoSelect == 1)
    {
        albedo = AlbedoTex.Sample(ClampedSampler, input.texCoord).rgb;
    }
    
    float diffuse = 0;
    for (uint i = 0; i < Scene.lightCount; ++i) {    
        float3 L = normalize(Lights[i].position - input.positionWS);

        diffuse += Lambert(N, L);
    }
            
    float3 color = (diffuse + Scene.ambient) * albedo;
    
    // Faux HDR tonemapping
	color = color / (color + float3(1, 1, 1));
       
    return float4(color, 1);
}
