#include "Config.hlsli"

ConstantBuffer<SceneData> Scene : register(RENDER_SCENE_DATA_REGISTER, SCENE_SPACE);
ConstantBuffer<ModelData> Model : register(RENDER_MODEL_DATA_REGISTER, MODEL_SPACE);

// -------------------------------------------------------------------------------------------------

float getSinVal(float z)
{
    float speed = 4.0;
    float t     = (Scene.time * 0.65);
    float s     = sin(z + t) * speed * z;
    return s;
}

// -------------------------------------------------------------------------------------------------

VSOutput vsmain(float4 position : POSITION, float3 color : COLOR)
{
    float4 VertPos      = position;
    
    float currZ         = VertPos.z;
    float prevZ         = currZ - 1.0;
    float currVal       = getSinVal(currZ * 0.03);
    float prevVal       = getSinVal(prevZ * 0.03);
    
    float3 currCenter   = float3(currVal, 0.0, currZ);
    float3 prevCenter   = float3(prevVal, 0.0, prevZ);
    float3 pathDir      = normalize(currCenter - prevCenter);
    float3 pathUp       = float3(0.0, 1.0, 0.0);
    float3 pathNormal   = cross(pathDir, pathUp);
    
    float3x3 basis      = float3x3(pathNormal, pathUp, pathDir);
    float3 at           = float3(basis[0][0], basis[1][0], basis[2][0]);
    float3 bt           = float3(basis[0][1], basis[1][1], basis[2][1]);
    float3 ct           = float3(basis[0][2], basis[1][2], basis[2][2]);
    float3x3 trans      = float3x3(at, bt, -ct);
    
    float3 spoke        = float3(VertPos.x, VertPos.y, 0.0);
    VertPos.xyz         = currCenter + mul(basis, spoke);
    
    float4x4 mvp = mul(Scene.viewProjectionMatrix, Model.modelMatrix);    
    
    VSOutput result = (VSOutput)0;
    result.position = mul(mvp, VertPos);
    result.color    = color;

    return result;
}

// -------------------------------------------------------------------------------------------------

float4 psmain(VSOutput input) : SV_TARGET
{
    return float4(input.color, 1);
}
