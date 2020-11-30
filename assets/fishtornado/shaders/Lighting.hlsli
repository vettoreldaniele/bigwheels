#ifndef LIGHTING_HLSLI
#define LIGHTING_HLSLI

#define CAUSTICS_IMAGE_COUNT 32

float Lambert(float3 N, float3 L)
{
    float diffuse = saturate(dot(N, L));
    return diffuse;
}

float Phong(float3 N, float3 L, float3 V, float hardness)
{
    float3 R        = reflect(-L, N);
    float  theta    = max(dot(R, V), 0);
    float  specular = pow(theta, hardness);
    return specular;
}

float BlinnPhong(float3 N, float3 L, float3 V, float hardness)
{
    float3 H        = normalize(L + V);
    float  theta    = saturate(dot(N, H));
    float  specular = pow(theta, hardness);
    return specular;
}

float3 CalculateCaustics(float time, float2 texCoord, Texture2DArray tex, SamplerState sam)
{
    float2 tc        = texCoord;
    float  tcz       = (float)((uint)time % CAUSTICS_IMAGE_COUNT);
    float  t         = 0.5 * time;
    float3 caustics1 = tex.Sample(sam, float3(tc * 0.035 + t *  0.005, tcz)).rgb;
    float3 caustics2 = tex.Sample(sam, float3(tc * 0.025 + t * -0.010, tcz)).rgb;
    float3 caustics3 = tex.Sample(sam, float3(tc * 0.010 + t * -0.002, tcz)).rgb;
    float3 caustics  = (caustics1 * caustics2 * 1.0 + caustics3 * 0.8);
    return caustics;
}

#endif // LIGHTING_HLSLI