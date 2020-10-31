#ifndef CONFIG_HLSLI
#define CONFIG_HLSLI

#define SCENE_DATA_SPACE         space0
#define MATERIAL_RESOURCES_SPACE space1
#define MATERIAL_DATA_SPACE      space2
#define MODEL_DATA_SPACE         space3

#define ALBEDO_TEXTURE_REGISTER     t0
#define ROUGHNESS_TEXTURE_REGISTER  t1
#define METALNESS_TEXTURE_REGISTER  t2
#define NORMAL_MAP_TEXTURE_REGISTER t3 
#define AMB_OCC_TEXTURE_REGISTER    t4
#define HEIGHT_MAP_TEXTURE          t5

#define CLAMPED_TEXTURE s8

#define PI 3.1415292

struct SceneData
{
    uint     frameNumber;
    float    time;    
    float4x4 viewProjectionMatrix;
    float3   eyePosition;
    uint     lightCount;
    float    ambient;
};

struct Light
{
    uint   type;
    float3 position;
    float3 color;
    float  intensity;
};

struct MaterialData
{
    float3 albedo;
    float  roughness;
    float  metalness;    
    uint   albedoSelect;    // 0 = value, 1 = texture
    uint   roughnessSelect; // 0 = value, 1 = texture
    uint   metalnessSelect; // 0 = value, 1 = texture
    uint   normalSelect;    // 0 = attrb, 1 = texture
};

struct ModelData
{
    float4x4 modelMatrix;
    float4x4 normalMatrix;
    float3   debugColor;
};

struct VSInput
{
    float3 position  : POSITION;
    float3 color     : COLOR;
    float3 normal    : NORMAL;
    float2 texCoord  : TEXCOORD;
    float3 tangent   : TANGENT;
    float3 bitangnet : BITANGENT;
};

struct VSOutput
{
    float4 position   : SV_POSITION;
    float3 positionWS : POSITIONWS;
    float3 color      : COLOR;
    float3 normal     : NORMAL;
    float2 texCoord   : TEXCOORD;
    float3 tangent    : TANGENT;
    float3 titangnet  : BITANGENT;
};

#endif // CONFIG_HLSLI
