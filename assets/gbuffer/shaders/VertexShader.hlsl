
#include "Config.hlsli"

#if defined(PPX_D3D11) // -----------------------------------------------------
cbuffer Scene : register(SCENE_CONSTANTS_REGISTER)
{
    SceneData Scene;
};

cbuffer Material : register(MATERIAL_CONSTANTS_REGISTER)
{
    MaterialData Material;
};

cbuffer Model : register(MODEL_CONSTANTS_REGISTER)
{
    ModelData Model;
};

StructuredBuffer<Light> Lights : register(LIGHT_DATA_REGISTER);
#else
ConstantBuffer<SceneData>    Scene    : register(b0, SCENE_DATA_SPACE);
StructuredBuffer<Light>      Lights   : register(t1, SCENE_DATA_SPACE);
ConstantBuffer<MaterialData> Material : register(b0, MATERIAL_DATA_SPACE);
ConstantBuffer<ModelData>    Model    : register(b0, MODEL_DATA_SPACE);
#endif

VSOutput vsmain(VSInput input)
{
    VSOutput output = (VSOutput)0;
    
    output.positionWS = mul(Model.modelMatrix, float4(input.position, 1)).xyz;
    output.position   = mul(Scene.viewProjectionMatrix, float4(output.positionWS, 1));
    output.normal     = mul(Model.normalMatrix, float4(input.normal, 0)).xyz;

    output.color    = input.color;
    output.texCoord = input.texCoord;
    
    // TBN
    output.normalTS    = mul(Model.modelMatrix, float4(input.normal, 0)).xyz;
    output.tangentTS   = mul(Model.modelMatrix, float4(input.tangent, 0)).xyz;
    output.bitangnetTS = mul(Model.modelMatrix, float4(input.bitangnet, 0)).xyz;    
    
    return output;
}