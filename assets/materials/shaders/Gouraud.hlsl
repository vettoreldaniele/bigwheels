
#include "Config.hlsli"

ConstantBuffer<SceneData>    Scene    : register(b0, SCENE_DATA_SPACE);
StructuredBuffer<Light>      Lights   : register(t1, SCENE_DATA_SPACE);
ConstantBuffer<MaterialData> Material : register(b0, MATERIAL_DATA_SPACE);
ConstantBuffer<ModelData>    Model    : register(b0, MODEL_DATA_SPACE);

Texture2D    AlbedoTex      : register(ALBEDO_TEXTURE_REGISTER, MATERIAL_RESOURCES_SPACE);
SamplerState ClampedSampler : register(CLAMPED_TEXTURE,         MATERIAL_RESOURCES_SPACE);

float Lambert(float3 N, float3 L)
{
    float diffuse = saturate(dot(N, L));
    return diffuse;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    float3 N = input.normal; 
    
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
