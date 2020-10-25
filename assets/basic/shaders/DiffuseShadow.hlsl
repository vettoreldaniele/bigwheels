//
// Keep things easy for now and use 16-byte aligned types
//
struct SceneData
{
    float4x4 ModelMatrix;  // Transforms object space to world space
    float4x4 NormalMatrix; // Transforms object space to normal space
    float4   Ambient;      // Object's ambient intensity
    
    float4x4 CameraViewProjectionMatrix; // Camera's view projection matrix
    
    float4   LightPosition;             // Light's position
    float4x4 LightViewProjectionMatrix; // Light's view projection matrix
};

ConstantBuffer<SceneData> Scene              : register(b0);
Texture2D                 ShadowDepthTexture : register(t1);
SamplerState              ShadowDepthSampler : register(s2);

struct VSOutput {
    float4 PositionWS : POSITION;
	float4 Position   : SV_POSITION;
	float3 Color      : COLOR;
    float3 Normal     : NORMAL;
    float4 PositionLS : POSITIONLS;
};

VSOutput vsmain(
    float4 Position : POSITION, 
    float3 Color    : COLOR, 
    float3 Normal   : NORMAL)
{
	VSOutput result;
    
    // Tranform input position into world space
    result.PositionWS = mul(Scene.ModelMatrix, Position);
    
    // Transform world space position into camera's view 
	result.Position = mul(Scene.CameraViewProjectionMatrix, result.PositionWS);
    
    // Color and normal
	result.Color  = Color;
    result.Normal = mul(Scene.NormalMatrix, float4(Normal, 0)).xyz;
    
    // Transform world space psoition into light's view
    result.PositionLS = mul(Scene.LightViewProjectionMatrix, result.PositionWS);
    
	return result;
}

float4 psmain(VSOutput input) : SV_TARGET
{
    float shadow = 0;
    float bias   = 0.002;    

    // Calculate shadow
    float2 shadowTexCoord = (float2)0;
    shadowTexCoord.x =  input.PositionLS.x / input.PositionLS.w / 2.0 + 0.5;
    shadowTexCoord.y = -input.PositionLS.y / input.PositionLS.w / 2.0 + 0.5;
    
    bool isInBoundsX = (shadowTexCoord.x >= 0) && (shadowTexCoord.x < 1);
    bool isInBoundsY = (shadowTexCoord.y >= 0) && (shadowTexCoord.y < 1);
    if (isInBoundsX && isInBoundsY) {
        // Read depth value from shadow texture
        float shadowDepth = ShadowDepthTexture.Sample(ShadowDepthSampler, shadowTexCoord).r;
        
        // Calculate depth from light's perspective
        float lightDepth = input.PositionLS.z / input.PositionLS.w;
        
        // Adjust for bias
        lightDepth -= bias;
        
        if (shadowDepth < lightDepth) {
            shadow = 0.5;
        }        
    }

    // Calculate diffuse lighting
    float3 L       = normalize(Scene.LightPosition.xyz - input.PositionWS.xyz);
    float3 N       = input.Normal;    
    float  diffuse = saturate(dot(N, L));
    
    // Final output color
    float  ambient = Scene.Ambient.x;       
    float3 Co       = (diffuse + ambient) * (1.0 - shadow) * input.Color;
	return float4(Co, 1);
}