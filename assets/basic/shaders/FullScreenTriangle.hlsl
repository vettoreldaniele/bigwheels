// 
// https://www.slideshare.net/DevCentralAMD/vertex-shader-tricks-bill-bilodeau
//

Texture2D    Tex0     : register(t0);
SamplerState Sampler0 : register(s1);

struct VSOutput {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VSOutput vsmain(uint id : SV_VertexID)
{
	VSOutput result;
    
    // Clip space position
	result.Position.x = (float)(id / 2) * 4.0 - 1.0;
    result.Position.y = (float)(id % 2) * 4.0 - 1.0;
    result.Position.z = 0.0;
    result.Position.w = 1.0;
    
    // Texture coordinates
	result.TexCoord.x = (float)(id / 2) * 2.0;
    result.TexCoord.y = 1.0 - (float)(id % 2) * 2.0;
    
	return result;
}

float4 psmain(VSOutput input) : SV_TARGET
{
	return Tex0.Sample(Sampler0, input.TexCoord);
}