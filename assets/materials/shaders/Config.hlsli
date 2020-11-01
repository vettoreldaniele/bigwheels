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
#define HEIGHT_MAP_TEXTURE_REGISTER t5
#define ENV_MAP_TEXTURE_REGISTER    t6
#define REFL_MAP_TEXTURE_REGISTER   t7

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
    uint   albedoSelect;     // 0 = value, 1 = texture
    uint   roughnessSelect;  // 0 = value, 1 = texture
    uint   metalnessSelect;  // 0 = value, 1 = texture
    uint   normalSelect;     // 0 = attrb, 1 = texture
    uint   iblSelect;        // 0 = white, 1 = texture
    uint   reflectionSelect; // 0 = none,  1 = texture
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
    float4 position    : SV_POSITION;
    float3 positionWS  : POSITIONWS;
    float3 color       : COLOR;
    float3 normal      : NORMAL;
    float2 texCoord    : TEXCOORD;
    float3 normalTS    : NORMALTS;
    float3 tangentTS   : TANGENTTS;
    float3 bitangnetTS : BITANGENTTS;
};

// circular atan2 - converts (x,y) on a unit circle to [0, 2pi]
//
#define catan2_epsilon 0.00001
#define catan2_NAN     0.0 / 0.0 // No gaurantee this is correct

float catan2(float y, float x)
{ 
    float absx = abs(x);
    float absy = abs(y);
    if ((absx < catan2_epsilon) && (absy < catan2_epsilon)) {
        return catan2_NAN;
    }
    else if ((absx > 0) && (absy == 0.0)) {
        return 0.0;
    }
    float s = 1.5 * 3.141592;
    if (y >= 0) {
        s = 3.141592 / 2.0;
    }
    return s - atan(x / y);
}

// Converts cartesian unit position 'pos' to (theta, phi) in
// spherical coodinates.
//
// theta is the azimuth angle between [0, 2pi].
// phi is the polar angle between [0, pi].
//
// NOTE: (0, 0, 0) will result in nan
//
float2 CartesianToSphereical(float3 pos)
{
    float absX = abs(pos.x);
    float absZ = abs(pos.z);
    // Handle pos pointing straight up or straight down
    if ((absX < 0.00001) && (absZ <= 0.00001)) {
        // Pointing straight up
        if (pos.y > 0) {
            return float2(0, 0);
        }
        // Pointing straight down
        else if (pos.y < 0) {
            return float2(0, 3.141592);
        }
        // Something went terribly wrong
        else {            
            return float2(catan2_NAN, catan2_NAN);
        }
    }
    float theta = catan2(pos.z, pos.x);
    float phi   = acos(pos.y);
    return float2(theta, phi);
}

#endif // CONFIG_HLSLI
