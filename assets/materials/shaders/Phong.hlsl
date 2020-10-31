
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

float4 psmain(VSOutput input) : SV_TARGET
{
    float3 N = input.normal;  
    float3 V = normalize(Scene.eyePosition.xyz - input.positionWS);
    
    float roughness = Material.roughness;
    if (Material.roughnessSelect == 1) {
        roughness = RoughnessTex.Sample(ClampedSampler, input.texCoord).r;
    }
    
    float metalness = Material.metalness;
    if (Material.metalnessSelect == 1) {
        metalness = MetalnessTex.Sample(ClampedSampler, input.texCoord).r;
    }
    
    float hardness = lerp(1.0f, 200.0f, saturate(roughness));
    
    float diffuse  = 0;
    float specular = 0;
	for (uint i = 0; i < Scene.lightCount; ++i) {    
        float3 L = normalize(Lights[i].position - input.positionWS);
        float  d = saturate(dot(N, L));
        diffuse += d;
        
        float3 R = reflect(-L, N);
        float  theta = saturate(dot(R, V));
        float  s = pow(theta, hardness);
        specular += s;
	}
        
    float3 color = AlbedoTex.Sample(ClampedSampler, input.texCoord).rgb;
    color = (diffuse + (specular * metalness) + Scene.ambient) * color;
    
    // HDR tonemapping
    color = color / (color + float3(1, 1, 1));
    
    return float4(color, 1);
}
