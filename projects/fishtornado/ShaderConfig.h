#ifndef SHADER_CONFIG_H
#define SHADER_CONFIG_H

#include "ppx/000_math_config.h"
using namespace ppx;

// -------------------------------------------------------------------------------------------------
// Scene
// -------------------------------------------------------------------------------------------------

namespace hlsl {

PPX_HLSL_PACK_BEGIN();
struct SceneData
{
    hlsl_float<4>     time;
    hlsl_float3<12>   eyePosition;
    hlsl_float4x4<64> viewMatrix;
    hlsl_float4x4<64> projectionMatrix;
    hlsl_float4x4<64> viewProjectionMatrix;
    hlsl_float<4>     fogNearDistance;
    hlsl_float<4>     fogFarDistance;
    hlsl_float<4>     fogPower;
};
PPX_HLSL_PACK_END();

} // namespace hlsl

// -------------------------------------------------------------------------------------------------
// Model
// -------------------------------------------------------------------------------------------------

namespace hlsl {

PPX_HLSL_PACK_BEGIN();
struct ModelData
{
    hlsl_float4x4<64> modelMatrix;
    hlsl_float4x4<64> mormalMatrix;
};
PPX_HLSL_PACK_END();

} // namespace hlsl

// -------------------------------------------------------------------------------------------------
// Flocking
// -------------------------------------------------------------------------------------------------

namespace hlsl {

PPX_HLSL_PACK_BEGIN();
struct FlockingData
{
    hlsl_int<4>     resX;
    hlsl_int<4>     resY;
    hlsl_float<4>   minThresh;
    hlsl_float<4>   maxThresh;
    hlsl_float<4>   minSpeed;
    hlsl_float<4>   maxSpeed;
    hlsl_float<4>   zoneRadius;
    hlsl_float<4>   time;
    hlsl_float<4>   timeDelta;
    hlsl_float3<16> predPos;
    hlsl_float3<16> camPos;
};
PPX_HLSL_PACK_END();

} // namespace hlsl

#endif // SHADER_CONFIG_H
