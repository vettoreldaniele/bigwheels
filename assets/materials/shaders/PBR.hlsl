
#include "Config.hlsli"

ConstantBuffer<SceneData>    Scene    : register(b0, SCENE_DATA_SPACE);
StructuredBuffer<Light>      Lights   : register(t1, SCENE_DATA_SPACE);
ConstantBuffer<MaterialData> Material : register(b0, MATERIAL_DATA_SPACE);
ConstantBuffer<ModelData>    Model    : register(b0, MODEL_DATA_SPACE);

Texture2D    AlbedoTex      : register(ALBEDO_TEXTURE_REGISTER,     MATERIAL_RESOURCES_SPACE);
Texture2D    RoughnessTex   : register(ROUGHNESS_TEXTURE_REGISTER,  MATERIAL_RESOURCES_SPACE);
Texture2D    MetalnessTex   : register(METALNESS_TEXTURE_REGISTER,  MATERIAL_RESOURCES_SPACE);
Texture2D    NormalMapTex   : register(NORMAL_MAP_TEXTURE_REGISTER, MATERIAL_RESOURCES_SPACE);
Texture2D    IBLTex         : register(IBL_MAP_TEXTURE_REGISTER,    MATERIAL_RESOURCES_SPACE);
Texture2D    EnvMapTex      : register(ENV_MAP_TEXTURE_REGISTER,    MATERIAL_RESOURCES_SPACE);
SamplerState ClampedSampler : register(CLAMPED_TEXTURE,             MATERIAL_RESOURCES_SPACE);

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * (float3)pow(1.0f - cosTheta, 5.0f);
}

float3 Environment(Texture2D tex, float3 coord)
{
    float2 uv = CartesianToSphereical(normalize(coord));
    uv.x = saturate(uv.x / (2.0 * PI));
    uv.y = saturate(uv.y / PI);
    float3 color = tex.Sample(ClampedSampler, uv).rgb;
    return color;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    float3 N  = input.normal;  
    float3 V  = normalize(Scene.eyePosition.xyz - input.positionWS);
    float  ao = 0.4f;    
    
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
    
    if (Material.normalSelect == 1) {
        float3   nTS = input.normalTS;
        float3   tTS = input.tangentTS;
        float3   bTS = input.bitangnetTS;
        float3x3 TBN = float3x3(tTS.x, bTS.x, nTS.x,
                                tTS.y, bTS.y, nTS.y,
                                tTS.z, bTS.z, nTS.z);
        
        // Read normal map value in tangent space
        float3 normal = (NormalMapTex.Sample(ClampedSampler, input.texCoord).rgb * 2.0f) - 1.0;

        // Transform from tangent space to world space
        N = mul(TBN, normal);
        
        // Normalize the transformed normal
        N = normalize(N);        
    }
    
    float3 P = input.positionWS;
    float3 R = reflect(-V, N);
    
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    //
    // float3 F0 = (float3)0.04;
    //
    float3 F0 = Material.F0;
    F0        = lerp(F0, albedo, metalness);
    
    float3 Lo = (float3)0.0;    
	for (uint i = 0; i < Scene.lightCount; ++i) {    
        float3 Lp    = Lights[i].position;
        float3 L     = normalize(Lp - P);
        float3 H     = normalize(V + L);

        // calculate per-light radiance
        //float  distance    = length(lightPositions[i] - WorldPos);
        //float  attenuation = 1.0 / (distance * distance);
        float3 radiance = (float3)1.0; //lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float  NDF = DistributionGGX(N, H, roughness);
        float  G   = GeometrySmith(N, V, L, roughness);
        float3 F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

        float3 nominator   = NDF * G * F;
        float  denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.00001f; // 0.001 to prevent divide by zero.
        float3 specular    = nominator / denominator;

        // kS is equal to Fresnel
        float3 kS = F;

        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        float3 kD = 1.0 - kS;

        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metalness;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}
    
    // ambient lighting (we now use IBL as the ambient term)
    float3 kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
    float3 kD = 1.0 - kS;
    kD *= 1.0 - metalness;
    
    float3 irradiance = (float3)1.0; 
    if (Material.iblSelect == 1) {
        irradiance = Environment(IBLTex, R) * Material.iblStrength;
    }
    
    float3 diffuse    = irradiance * albedo +  Scene.ambient;
    float3 ambient    = (kD * diffuse) * ao;    
        
    // Environment reflection
    float3 reflection = (float3)0.0;
    if (Material.envSelect) {
        reflection = Environment(EnvMapTex, R) * Material.envStrength;
    }    
    
    // Final color
    float3 color = ambient + Lo + (kS * reflection * (1.0 - roughness));
    
    // Faux HDR tonemapping
    color = color / (color + float3(1, 1, 1));
    
    return float4(color, 1);
}
