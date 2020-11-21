
#include "Config.hlsli"

ConstantBuffer<SceneData>    Scene    : register(b0, SCENE_DATA_SPACE);
StructuredBuffer<Light>      Lights   : register(t1, SCENE_DATA_SPACE);
ConstantBuffer<MaterialData> Material : register(b0, MATERIAL_DATA_SPACE);
ConstantBuffer<ModelData>    Model    : register(b0, MODEL_DATA_SPACE);

Texture2D    AlbedoTex      : register(ALBEDO_TEXTURE_REGISTER,     MATERIAL_RESOURCES_SPACE);
Texture2D    RoughnessTex   : register(ROUGHNESS_TEXTURE_REGISTER,  MATERIAL_RESOURCES_SPACE);
Texture2D    MetalnessTex   : register(METALNESS_TEXTURE_REGISTER,  MATERIAL_RESOURCES_SPACE);
Texture2D    NormalMapTex   : register(NORMAL_MAP_TEXTURE_REGISTER, MATERIAL_RESOURCES_SPACE);
SamplerState ClampedSampler : register(CLAMPED_TEXTURE,             MATERIAL_RESOURCES_SPACE);

float Lambert(float3 N, float3 L)
{
    float diffuse = saturate(dot(N, L));
    return diffuse;
}

float BlinnPhong(float3 N, float3 L, float3 V, float hardness)
{
    float3 H        = normalize(L + V);
    float  theta    = saturate(dot(N, H));
    float  specular = pow(theta, hardness);
    return specular;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    float3 N = normalize(input.normal);  
    float3 V = normalize(Scene.eyePosition.xyz - input.positionWS);

    float3 albedo = Material.albedo;
    if (Material.albedoSelect == 1) {
        albedo = AlbedoTex.Sample(ClampedSampler, input.texCoord).rgb;
    }
    
    float roughness = Material.roughness;
    if (Material.roughnessSelect == 1) {
        roughness = RoughnessTex.Sample(ClampedSampler, input.texCoord).r;
    }
    
    float metalness = Material.metalness;
    if (Material.metalnessSelect == 1) {
        metalness = MetalnessTex.Sample(ClampedSampler, input.texCoord).r;
    }    
    
    float hardness = lerp(1.0, 100.0, 1.0 - saturate(roughness));
    
    float diffuse  = 0;
    float specular = 0;
    for (uint i = 0; i < Scene.lightCount; ++i) {    
        float3 L = normalize(Lights[i].position - input.positionWS);

        diffuse += Lambert(N, L);
        
        specular += BlinnPhong(N, L, V, hardness);
    }
        
    float3 color = (diffuse + (specular * metalness) + Scene.ambient) * albedo;
    
    // Faux HDR tonemapping
    color = color / (color + float3(1, 1, 1));
    
    return float4(color, 1);
}
