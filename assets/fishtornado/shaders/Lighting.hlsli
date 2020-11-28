#ifndef LIGHTING_HLSLI
#define LIGHTING_HLSLI

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

#endif // LIGHTING_HLSLI