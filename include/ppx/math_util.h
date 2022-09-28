#ifndef ppx_math_util_h
#define ppx_math_util_h

#include "ppx/math_config.h"

namespace ppx {

// Converts spherical coordinate 'sc' to unit cartesian position.
//
// theta is the azimuth angle between [0, 2pi].
// phi is the polar angle between [0, pi].
//
// theta = 0, phi =[0, pi] sweeps the positive X axis:
//    SphericalToCartesian(0, 0)    = (0,  1, 0)
//    SphericalToCartesian(0, pi/2) = (1,  0, 0)
//    SphericalToCartesian(0, pi)   = (0, -1, 0)
//
// theta = [0, 2pi], phi = [pi/2] sweeps a circle:
//    SphericalToCartesian(0,     pi/2) = ( 1, 0, 0)
//    SphericalToCartesian(pi/2,  pi/2) = ( 0, 0, 1)
//    SphericalToCartesian(pi  ,  pi/2) = (-1, 0, 0)
//    SphericalToCartesian(3pi/2, pi/2) = ( 0, 0,-1)
//    SphericalToCartesian(2pi,   pi/2) = ( 1, 0, 0)
//
inline float3 SphericalToCartesian(float theta, float phi)
{
    return float3(
        cos(theta) * sin(phi), // x
        cos(phi),              // y
        sin(theta) * sin(phi)  // z
    );
}

inline float3 SphericalTangent(float theta, float phi)
{
    return float3(
        sin(theta), // x
        0,          // y
        -cos(theta) // z
    );
}

} // namespace ppx

#endif // ppx_math_util_h
