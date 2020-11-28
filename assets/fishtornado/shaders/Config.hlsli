#ifndef CONFIG_HLSLI
#define CONFIG_HLSLI

#define SCENE_SPACE    space0
#define MODEL_SPACE    space1
#define MATERIAL_SPACE space2
#define FLOCKING_SPACE space2
                                           
#define RENDER_SCENE_DATA_REGISTER                 b0 // SCENE_SPCE
#define RENDER_MODEL_DATA_REGISTER                 b0 // MODEL_SPACE
#define RENDER_MATERIAL_DATA_REGISTER              b0 // MATERIAL_SPACE
#define RENDER_ALBEDO_TEXTURE_REGISTER             t1 // MATERIAL_SPACE
#define RENDER_ROUGHNESS_TEXTURE_REGISTER          t1 // MATERIAL_SPACE
#define RENDER_NORMAL_MAP_TEXTURE_REGISTER         t1 // MATERIAL_SPACE
#define RENDER_FLOCKING_DATA_REGISTER              b0 // FLOCKING_SPACE
#define RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER  t1 // FLOCKING_SPACE
#define RENDER_CURRENT_POSITION_TEXTURE_REGISTER   t2 // FLOCKING_SPACE
#define RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER   t3 // FLOCKING_SPACE

// -------------------------------------------------------------------------------------------------
// VS/PS Input and Output
// -------------------------------------------------------------------------------------------------

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
    float4 position    : SV_POSITION;
    float3 positionWS  : POSITIONWS;
    float3 color       : COLOR;
    float3 normal      : NORMAL;
    float2 texCoord    : TEXCOORD;
    float3 normalTS    : NORMALTS;
    float3 tangentTS   : TANGENTTS;
    float3 bitangnetTS : BITANGENTTS;
};

// -------------------------------------------------------------------------------------------------
// Scene
// -------------------------------------------------------------------------------------------------

struct SceneData
{
    float    time;
    float3   eyePosition;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 viewProjectionMatrix;
    float    fogNearDistance;
    float    fogFarDistance;
    float    fogPower;    
};

// -------------------------------------------------------------------------------------------------
// Model
// -------------------------------------------------------------------------------------------------

struct ModelData
{
    float4x4 modelMatrix;
    float4x4 mormalMatrix;
};

// -------------------------------------------------------------------------------------------------
// Flocking
// -------------------------------------------------------------------------------------------------

struct FlockingData
{
    int    resX;
    int    resY;
    float  minThresh;
    float  maxThresh;
    float  minSpeed;
    float  maxSpeed;
    float  zoneRadius;
    float  time;
    float  timeDelta;
    float3 predPos;
    float3 camPos;
};

// -------------------------------------------------------------------------------------------------
// Material
// -------------------------------------------------------------------------------------------------

struct MaterialData
{
    float  F0;
    float3 albedo;
    float  roughness;
    float  metalness;
    float  iblStrength;
    float  envStrength;
    uint   albedoSelect;     // 0 = value, 1 = texture
    uint   roughnessSelect;  // 0 = value, 1 = texture
    uint   metalnessSelect;  // 0 = value, 1 = texture
    uint   normalSelect;     // 0 = attrb, 1 = texture
};


#endif // CONFIG_HLSLI