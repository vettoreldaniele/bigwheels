struct Coord
{
    // Original image coordinate.
    float2 xy;

    // Normalized coordinate for pixel being filtered.
    float2 uv;

    // Left, right, top and bottom from the pixel coordinate.
    float2 uvL;
    float2 uvR;
    float2 uvT;
    float2 uvB;
};

Coord GetPixelCoord(float2 xy, float2 topLeft, float2 texelSize)
{
    Coord coord;
    coord.xy  = xy + topLeft;
    coord.uv  = coord.xy * texelSize;
    coord.uvL = coord.uv - float2(texelSize.x, 0);
    coord.uvR = coord.uv + float2(texelSize.x, 0);
    coord.uvT = coord.uv - float2(0, texelSize.y);
    coord.uvB = coord.uv + float2(0, texelSize.y);
    return coord;
}

struct CSInput
{
    float2 filterCenter;
    float2 filterArea;
    float2 filterTopLeft;
    float2 texelSize;

    // Intensity - Used by bloom_final.hlsl.
    float intensity;

    // Curve and threshold - used by bloom_prefilter.hlsl.
    float3 curve;
    float  threshold;

    // Aspect ratio - used by checkerboard.hlsl and splat.hlsl.
    float aspectRatio;

    // Clear value - used by clear.hlsl.
    float clearValue;

    // Color value - used by color.hlsl and splat.hlsl.
    float4 color;

    // Dither scale - used by display.hlsl.
    float2 ditherScale;

    // Dye texel size and dissipation - used by advection.hlsl.
    float2 dyeTexelSize;
    float  dissipation;

    // used by advection.hlsl and vorticity.hlsl
    float  dt;

    // Coordinate and radius - used by splat.hlsl.
    float2 coordinate;
    float  radius;

    // Weight - used by sunrays.hlsl.
    float weight;

    // curl - used by vorticity.hlsl.
    float curl;
};

#if defined(PPX_D3D11)
cbuffer Param : register(b0)
{
    CSInput Params;
}
#else
// ConstantBuffer is available in SM5.1 and above.
ConstantBuffer<CSInput> Params : register(b0);
#endif

SamplerState        Sampler : register(s1);
Texture2D           InputTexture : register(t2);
RWTexture2D<float4> OutputTexture : register(u3);
