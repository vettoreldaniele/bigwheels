#include "config.hlsli"

#define SHADING
#define BLOOM
#define SUNRAYS

SamplerState ClampedSampler : register(s0);

struct PSInput
{
    float2 vUv : VUV;
    float2 vL : VL;
    float2 vR : VR;
    float2 vT : VT;
    float2 vB : VB;
};

Texture2D uTexture : register(t0);

// FIXME(dnovillo) - These textures will need to be used.
#if 0
Texture2D uBloom : register(t1);
Texture2D uSunrays : register(t2);
Texture2D uDithering : register(t3);
#endif

float3 linearToGamma(float3 color)
{
    color = max(color, float3(0, 0, 0));
    return max(1.055 * pow(color, float3(0.416666667, 0.416666667, 0.416666667)) - 0.055, float3(0, 0, 0));
}

[numthreads(1, 1, 1)] void csmain(uint2 tid
                                  : SV_DispatchThreadID) {
    Coord  coord = GetPixelCoord(tid, Params.filterTopLeft, Params.texelSize);
    float3 c     = InputTexture.SampleLevel(Sampler, coord.uv, 0).rgb;

#ifdef SHADING
    float3 lc = InputTexture.SampleLevel(Sampler, coord.uvL, 0).rgb;
    float3 rc = InputTexture.SampleLevel(Sampler, coord.uvR, 0).rgb;
    float3 tc = InputTexture.SampleLevel(Sampler, coord.uvT, 0).rgb;
    float3 bc = InputTexture.SampleLevel(Sampler, coord.uvB, 0).rgb;

    float dx = length(rc) - length(lc);
    float dy = length(tc) - length(bc);

    float3 n = normalize(float3(dx, dy, length(Params.texelSize)));
    float3 l = float3(0.0, 0.0, 1.0);

    float diffuse = clamp(dot(n, l) + 0.7, 0.7, 1.0);
    c *= diffuse;
#endif

#ifdef BLOOM
    // FIXME(dnovillo) - This should sample from uBloom.
    float3 bloom = InputTexture.SampleLevel(Sampler, coord.uv, 0).rgb;
#endif

#ifdef SUNRAYS
    // FIXME(dnovillo) - This should sample from uSunrays.
    float sunrays = InputTexture.SampleLevel(Sampler, coord.uv, 0).r;
    c *= sunrays;
#ifdef BLOOM
    bloom *= sunrays;
#endif
#endif

#ifdef BLOOM
    // FIXME(dnovillo) - This should sample from uDithering.
    float noise = InputTexture.SampleLevel(Sampler, coord.uv * Params.ditherScale, 0).r;
    noise       = noise * 2.0 - 1.0;
    bloom += noise / 255.0;
    bloom = linearToGamma(bloom);
    c += bloom;
#endif

    float a                 = max(c.r, max(c.g, c.b));
    OutputTexture[coord.xy] = float4(c, a);
}
