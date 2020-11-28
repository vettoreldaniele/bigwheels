#include "Config.hlsli"
#include "Lighting.hlsli"

ConstantBuffer<SceneData>    Scene            : register(RENDER_SCENE_DATA_REGISTER,                SCENE_SPACE);
ConstantBuffer<ModelData>    Model            : register(RENDER_MODEL_DATA_REGISTER,                MODEL_SPACE);
ConstantBuffer<MaterialData> Material         : register(RENDER_MATERIAL_DATA_REGISTER,             MATERIAL_SPACE);
Texture2D                    AlbedoTexture    : register(RENDER_ALBEDO_TEXTURE_REGISTER,            MATERIAL_SPACE);
Texture2D                    RoughnessTexture : register(RENDER_ROUGHNESS_TEXTURE_REGISTER,         MATERIAL_SPACE);
Texture2D                    NormalMapTexture : register(RENDER_NORMAL_MAP_TEXTURE_REGISTER,        MATERIAL_SPACE);
SamplerState                 ClampedSampler   : register(RENDER_CLAMPED_SAMPLER_REGISTER,    MATERIAL_SPACE);
ConstantBuffer<FlockingData> Flocking         : register(RENDER_FLOCKING_DATA_REGISTER,             FLOCKING_SPACE);
Texture2D<float4>            PrevPositionTex  : register(RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, FLOCKING_SPACE);
Texture2D<float4>            CurrPositionTex  : register(RENDER_CURRENT_POSITION_TEXTURE_REGISTER,  FLOCKING_SPACE);
Texture2D<float4>            CurrVelocityTex  : register(RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER,  FLOCKING_SPACE);

// -------------------------------------------------------------------------------------------------

float getSinVal(float z, float randPer)
{
    //float t = (Scene.time * 0.75 + randPer * 26.0);
    float t = (Scene.time * 1.5 + randPer * 32.0);
    float s = sin(z + t) * z;
    return s;
}

// -------------------------------------------------------------------------------------------------

VSOutput vsmain(VSInput input, uint instanceId : SV_InstanceID)
{
    // Texture data
    uint2  xy        = uint2((instanceId % Flocking.resX), (instanceId / Flocking.resY));
    float4 prevPos   = PrevPositionTex[xy];
    float4 currPos   = CurrPositionTex[xy];
    float4 currVel   = CurrVelocityTex[xy];
    float  leader    = currPos.a;

    float currZ = input.position.z;
    float prevZ = currZ - 1.0;
    float currVal = getSinVal(currZ * 0.2, leader + float(instanceId));
    float prevVal = getSinVal(prevZ * 0.2, leader + float(instanceId));

    float3 currCenter = float3(currVal, 0.0, currZ);
    float3 prevCenter = float3(prevVal, 0.0, prevZ);
    float3 pathDir    = normalize(currCenter - prevCenter);
    float3 pathUp     = float3(0.0, 1.0, 0.0);
    float3 pathNormal = normalize(cross(pathUp, pathDir));
    float3x3 basis    = float3x3(pathNormal, pathUp, pathDir);

    float3 spoke   = float3(input.position.xy, 0.0);
    float3 VertPos = currCenter + mul(basis, spoke);

    // Calculate matrix
    float3   worldUp = float3(0, 1, 0);
    //float3   dir     = normalize(currPos.xyz - prevPos.xyz); 
    float3   dir     = normalize(currVel.xyz);
    float3   right   = normalize(cross(dir, worldUp));
    float3   up      = normalize(cross(right, dir));
    float3x3 m       = float3x3(right, up, dir);

    // Set final vertex
    VertPos = mul(m, VertPos);
    VertPos.xyz += currPos.xyz;

    // Position in view space
    float4 VertPosVS = mul(Scene.viewMatrix, float4(VertPos, 1.0));
    
    // NOTE: modelMatrix is an identity matrix for flocking
    //
    float4x4 mvpMatrix    = mul(Scene.viewProjectionMatrix, Model.modelMatrix);
    float3x3 normalMatrix = mul((float3x3)Model.modelMatrix, m);

    VSOutput result    = (VSOutput)0;
    result.position    = mul(mvpMatrix, float4(VertPos, 1.0));
    result.positionWS  = VertPos;
    result.color       = input.color;
    result.normal      = mul(normalMatrix, input.normal);
    result.texCoord    = input.texCoord;
    result.normalTS    = mul(normalMatrix, input.normal);
    result.tangentTS   = mul(normalMatrix, input.tangent);
    result.bitangnetTS = mul(normalMatrix, input.bitangnet);
    result.fogAmount   = pow(1.0 - min(max(length(VertPosVS.xyz) - Scene.fogNearDistance, 0.0) / Scene.fogFarDistance, 1.0), Scene.fogPower);

    return result;
}

// -------------------------------------------------------------------------------------------------

float4 psmain(VSOutput input) : SV_TARGET
{
    float3 P  = input.positionWS.xyz;
    float3 N  = normalize(input.normal);
    float3 V  = normalize(Scene.eyePosition.xyz - input.positionWS);
    
    float3   nTS = normalize(input.normalTS);
    float3   tTS = normalize(input.tangentTS);
    float3   bTS = normalize(input.bitangnetTS);
    float3x3 TBN = float3x3(tTS.x, bTS.x, nTS.x,
                            tTS.y, bTS.y, nTS.y,
                            tTS.z, bTS.z, nTS.z);
    N = (NormalMapTexture.Sample(ClampedSampler, input.texCoord).rgb * 2.0f) - 1.0;
    N = mul(TBN, N);
    N = normalize(N);

    float3 Lp = Scene.lightPosition; 
    float3 L =  normalize(Lp - P);
    
    float roughness = RoughnessTexture.Sample(ClampedSampler, input.texCoord).r;
    float hardness  = lerp(5.0, 30.0, 1.0 - saturate(roughness));
    
    float diffuse  = Lambert(N, L);
    float specular = 0.7 * BlinnPhong(N, L, V, roughness);
    
    float3 color = AlbedoTexture.Sample(ClampedSampler, input.texCoord).rgb;
    color = color * ((float3)(diffuse + specular) + Scene.ambient);
    
    color = lerp(Scene.fogColor, color, input.fogAmount);
        
    return float4(color, 1);
}