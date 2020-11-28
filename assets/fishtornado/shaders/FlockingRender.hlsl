#include "Config.hlsli"

ConstantBuffer<SceneData>    Scene           : register(RENDER_SCENE_DATA_REGISTER,                SCENE_SPACE);
ConstantBuffer<ModelData>    Model           : register(RENDER_MODEL_DATA_REGISTER,                MODEL_SPACE);
ConstantBuffer<FlockingData> Flocking        : register(RENDER_FLOCKING_DATA_REGISTER,             FLOCKING_SPACE);
Texture2D<float4>            PrevPositionTex : register(RENDER_PREVIOUS_POSITION_TEXTURE_REGISTER, FLOCKING_SPACE);
Texture2D<float4>            CurrPositionTex : register(RENDER_CURRENT_POSITION_TEXTURE_REGISTER,  FLOCKING_SPACE);
Texture2D<float4>            CurrVelocityTex : register(RENDER_CURRENT_VELOCITY_TEXTURE_REGISTER,  FLOCKING_SPACE);

// -------------------------------------------------------------------------------------------------

float getSinVal(float z, float randPer)
{
    //float t = (Scene.time * 0.75 + randPer * 26.0);
    float t = (Scene.time * 1.5 + randPer * 32.0);
    float s = sin(z + t) * z;
    return s;
}

// -------------------------------------------------------------------------------------------------

VSOutput vsmain(float4 position : POSITION, float3 color : COLOR, uint instanceId : SV_InstanceID)
{
    // Texture data
    uint2  xy        = uint2((instanceId % Flocking.resX), (instanceId / Flocking.resY));
    float4 prevPos   = PrevPositionTex[xy];
    float4 currPos   = CurrPositionTex[xy];
    float4 currVel   = CurrVelocityTex[xy];
    float  leader    = currPos.a;

    float currZ = position.z;
    float prevZ = currZ - 1.0;
    float currVal = getSinVal(currZ * 0.2, leader + float(instanceId));
    float prevVal = getSinVal(prevZ * 0.2, leader + float(instanceId));

    float3 currCenter = float3(currVal, 0.0, currZ);
    float3 prevCenter = float3(prevVal, 0.0, prevZ);
    float3 pathDir    = normalize(currCenter - prevCenter);
    float3 pathUp     = float3(0.0, 1.0, 0.0);
    float3 pathNormal = normalize(cross(pathUp, pathDir));
    float3x3 basis    = float3x3(pathNormal, pathUp, pathDir);

    float3 VertPos = position.xyz;
    float3 spoke   = float3(VertPos.x, VertPos.y, 0.0);
    VertPos.xyz    = currCenter + mul(basis, spoke);

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

    float4x4 mvp = mul(Scene.viewProjectionMatrix, Model.modelMatrix);

    VSOutput result = (VSOutput)0;
    result.position = mul(mvp, float4(VertPos, position.w));
    result.color    = color;

    return result;    
    
/*    
    // texture data
    //float2  samplePos = float2((instanceId % Flocking.resX), (instanceId / Flocking.resY));
    uint2  xy        = uint2((instanceId % Flocking.resX), (instanceId / Flocking.resY));
    float4 prevPos   = PrevPositionTex[xy]; //texture(uPrevPosTex, samplePos);
    float4 currPos   = CurrPositionTex[xy]; //texture(uCurrPosTex, samplePos);
    float4 currVel   = CurrVelocityTex[xy]; //texture(uCurrVelTex, samplePos);
    float  leader    = currPos.a;
    //vCrowd          = (1.0 - pow(min(currVel.a * 0.015, 1.0), 0.5)) + 0.25;

    float currZ = position.z;
    float prevZ = currZ - 1.0;
    // float speed = length( currVel.xyz ) * 0.075;
    float currVal = getSinVal(currZ * 0.3, leader + float(instanceId)); // currZ * speed
    //float prevVal = getSinVal(prevZ * 0.2, leader); // prevZ * speed

    float3 currCenter = float3(currVal, 0.0, currZ);
   // float3 prevCenter = float3(prevVal, 0.0, prevZ);
    //float3 pathDir    = normalize(-currVel); //normalize(currCenter - prevCenter);
    //float3 pathUp     = float3(0.0, 1.0, 0.0);
    //float3 pathNormal = normalize(cross(pathDir, pathUp));

    //float3x3 basis = float3x3(pathNormal, pathUp, pathDir);
    //float3   at    = float3(basis[0][0], basis[1][0], basis[2][0]);
    //float3   bt    = float3(basis[0][1], basis[1][1], basis[2][1]);
    //float3   ct    = float3(basis[0][2], basis[1][2], basis[2][2]);
    //float3x3 trans = float3x3(at, bt, -ct);

    //float3 spoke  = float3(VertPos.x, VertPos.y, 0.0);
    //VertPos.xyz   = currCenter + mul(basis, spoke);

    // Calculate matrix
    //float3   dir   = normalize(currVel.xyz); //normalize( currPos.xyz - prevPos.xyz );
    float3   worldUp = float3(0, 1, 0);
    //float3   dir     = normalize(currPos.xyz - prevPos.xyz); 
    float3   dir     = normalize(currVel.xyz);
    float3   right   = normalize(cross(dir, worldUp));
    float3   up      = normalize(cross(right, dir));
    float3x3 m       = float3x3(right, up, dir);

    //// Set final vertex
    //float3 vertexPos = mul(m, VertPos.xyz);
    //VertPos          = float4(vertexPos + currPos.xyz, position.w);
   
    float3 VertPos =  mul(m, position.xyz);
    VertPos += currPos.xyz;
    VertPos += currCenter;

    float4x4 mvp = mul(Scene.viewProjectionMatrix, Model.modelMatrix);

    VSOutput result = (VSOutput)0;
    result.position = mul(mvp, float4(VertPos, position.w));
    result.color    = color;

    return result;
*/
}

// -------------------------------------------------------------------------------------------------

float4 psmain(VSOutput input)
    : SV_TARGET
{
    return float4(input.color, 1);
}
