
#include "Config.hlsli"

ConstantBuffer<SceneData>    Scene    : register(b0, SCENE_DATA_SPACE);
StructuredBuffer<Light>      Lights   : register(t1, SCENE_DATA_SPACE);
ConstantBuffer<MaterialData> Material : register(b0, MATERIAL_DATA_SPACE);
ConstantBuffer<ModelData>    Model    : register(b0, MODEL_DATA_SPACE);

VSOutput vsmain(VSInput input)
{
    VSOutput output = (VSOutput)0;
    
    output.positionWS = mul(Model.modelMatrix, float4(input.position, 1)).xyz;
    output.position   = mul(Scene.viewProjectionMatrix, float4(output.positionWS, 1));
    output.normal     = normalize(mul(Model.normalMatrix, float4(input.normal, 0)).xyz);

    output.color    = input.color;
    output.texCoord = input.texCoord;
    
    // TBN
    output.normalTS    = normalize(mul(Model.modelMatrix, float4(input.normal, 0)).xyz);
    output.tangentTS   = normalize(mul(Model.modelMatrix, float4(input.tangent, 0)).xyz);
    output.bitangnetTS = normalize(mul(Model.modelMatrix, float4(input.bitangnet, 0)).xyz);    
    
    return output;
}