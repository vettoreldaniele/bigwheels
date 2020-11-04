
#include "GBuffer.hlsli"
#include "Config.hlsli"

ConstantBuffer<SceneData>    Scene    : register(b0, SCENE_DATA_SPACE);
StructuredBuffer<Light>      Lights   : register(t1, SCENE_DATA_SPACE);
ConstantBuffer<MaterialData> Material : register(b0, MATERIAL_DATA_SPACE);
ConstantBuffer<ModelData>    Model    : register(b0, MODEL_DATA_SPACE);

Texture2D    AlbedoTex      : register(ALBEDO_TEXTURE_REGISTER,     MATERIAL_RESOURCES_SPACE);
Texture2D    RoughnessTex   : register(ROUGHNESS_TEXTURE_REGISTER,  MATERIAL_RESOURCES_SPACE);
Texture2D    MetalnessTex   : register(METALNESS_TEXTURE_REGISTER,  MATERIAL_RESOURCES_SPACE);
Texture2D    NormalMapTex   : register(NORMAL_MAP_TEXTURE_REGISTER, MATERIAL_RESOURCES_SPACE);
Texture2D    EnvMapTex      : register(ENV_MAP_TEXTURE_REGISTER,    MATERIAL_RESOURCES_SPACE);
Texture2D    ReflMapTex     : register(REFL_MAP_TEXTURE_REGISTER,   MATERIAL_RESOURCES_SPACE);
SamplerState ClampedSampler : register(CLAMPED_TEXTURE,             MATERIAL_RESOURCES_SPACE);

PackedGBuffer psmain(VSOutput input)
{
    GBuffer gbuffer = (GBuffer)0;
    gbuffer.position     = input.positionWS;
    
    float3 normal = input.normal;
    if (Material.normalSelect == 1) {
        float3   nTS = input.normalTS;
        float3   tTS = input.tangentTS;
        float3   bTS = input.bitangnetTS;
        float3x3 TBN = float3x3(tTS.x, bTS.x, nTS.x,
                                tTS.y, bTS.y, nTS.y,
                                tTS.z, bTS.z, nTS.z);
        
        // Read normal map value in tangent space
        normal = (NormalMapTex.Sample(ClampedSampler, input.texCoord).rgb * 2.0f) - 1.0;

        // Transform from tangent space to world space
        normal = mul(TBN, normal);
        
        // Normalize the transformed normal
        normal = normalize(normal);        
    }    
    gbuffer.normal = normal;
    
	gbuffer.albedo = Material.albedo;
	if (Material.albedoSelect == 1) {
		gbuffer.albedo = AlbedoTex.Sample(ClampedSampler, input.texCoord).rgb;
	}
    
    gbuffer.F0 = Material.F0;
    
    gbuffer.roughness = Material.roughness;
    if (Material.roughnessSelect == 1) {
        gbuffer.roughness = RoughnessTex.Sample(ClampedSampler, input.texCoord).r;
    }
    
    gbuffer.metalness = Material.metalness;
    if (Material.metalnessSelect == 1) {
        gbuffer.metalness = MetalnessTex.Sample(ClampedSampler, input.texCoord).r;
    }
    
    gbuffer.ambOcc       = 1;    
    gbuffer.iblStrength  = (Material.iblSelect == 1) ? Material.iblStrength : 0.0;
    gbuffer.reflStrength = (Material.envSelect == 1) ? Material.envStrength : 0.0;
        
    PackedGBuffer packed = PackGBuffer(gbuffer);
    return packed;
}