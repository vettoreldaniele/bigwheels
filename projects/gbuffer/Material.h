#ifndef MATERIAL_H
#define MATERIAL_H

#include "ppx/000_math_config.h"
#include "ppx/fs.h"
#include "ppx/grfx/grfx_pipeline.h"

using ppx::float3;
using ppx::hlsl_float;
using ppx::hlsl_float2;
using ppx::hlsl_float3;
using ppx::hlsl_float4;
using ppx::hlsl_int;
using ppx::hlsl_int2;
using ppx::hlsl_int3;
using ppx::hlsl_uint;
using ppx::hlsl_uint2;
using ppx::hlsl_uint3;
using ppx::hlsl_uint4;

extern const float3 F0_MetalTitanium;
extern const float3 F0_MetalChromium;
extern const float3 F0_MetalIron;
extern const float3 F0_MetalNickel;
extern const float3 F0_MetalPlatinum;
extern const float3 F0_MetalCopper;
extern const float3 F0_MetalPalladium;
extern const float3 F0_MetalZinc;
extern const float3 F0_MetalGold;
extern const float3 F0_MetalAluminum;
extern const float3 F0_MetalSilver;
extern const float3 F0_DiletricWater;
extern const float3 F0_DiletricPlastic;
extern const float3 F0_DiletricGlass;
extern const float3 F0_DiletricCrystal;
extern const float3 F0_DiletricGem;
extern const float3 F0_DiletricDiamond;

PPX_HLSL_PACK_BEGIN();
struct MaterialConstants
{
    hlsl_float<4>   F0;
    hlsl_float3<12> albedo;
    hlsl_float<4>   roughness;
    hlsl_float<4>   metalness;
    hlsl_float<4>   iblStrength;
    hlsl_float<4>   envStrength;
    hlsl_uint<4>    albedoSelect;
    hlsl_uint<4>    roughnessSelect;
    hlsl_uint<4>    metalnessSelect;
    hlsl_uint<4>    normalSelect;
};
PPX_HLSL_PACK_END();

struct MaterialCreateInfo
{
    float         F0;
    float3        albedo;
    float         roughness;
    float         metalness;
    float         iblStrength;
    float         envStrength;
    ppx::fs::path albedoTexturePath;
    ppx::fs::path roughnessTexturePath;
    ppx::fs::path metalnessTexturePath;
    ppx::fs::path normalTexturePath;
};

class Material
{
public:
    Material() {}
    virtual ~Material() {}

    ppx::Result Create(ppx::grfx::Queue* pQueue, ppx::grfx::DescriptorPool* pPool, const MaterialCreateInfo* pCreateInfo);
    void        Destroy();

    static ppx::Result CreateMaterials(ppx::grfx::Queue* pQueue, ppx::grfx::DescriptorPool* pPool);
    static void        DestroyMaterials();

    static Material* GetMaterialRustedIron() { return &sRustedIron; }
    static Material* GetMaterialPaintedMetal() { return &sPaintedMetal; }
    static Material* GetMaterialCopper() { return &sCopper; }
    static Material* GetMaterialGold() { return &sGold; }
    static Material* GetMaterialTitanium() { return &sTitanium; }
    static Material* GetMaterialWhiteRoughPlastic() { return &sWhiteRoughPlastic; }
    static Material* GetMaterialStoneTile() { return &sStoneTile; }

    static ppx::grfx::DescriptorSetLayout* GetMaterialResourcesLayout() { return sMaterialResourcesLayout.Get(); }
    static ppx::grfx::DescriptorSetLayout* GetMaterialDataLayout() { return sMaterialDataLayout.Get(); }

    ppx::grfx::DescriptorSet* GetMaterialResourceSet() const { return mMaterialResourcesSet.Get(); }
    ppx::grfx::DescriptorSet* GetMaterialDataSet() const { return mMaterialDataSet.Get(); }

private:
    ppx::grfx::BufferPtr        mMaterialConstants;
    ppx::grfx::TexturePtr       mAlbedoTexture;
    ppx::grfx::TexturePtr       mRoughnessTexture;
    ppx::grfx::TexturePtr       mMetalnessTexture;
    ppx::grfx::TexturePtr       mNormalMapTexture;
    ppx::grfx::DescriptorSetPtr mMaterialResourcesSet;
    ppx::grfx::DescriptorSetPtr mMaterialDataSet;

    static ppx::grfx::TexturePtr             s1x1BlackTexture;
    static ppx::grfx::TexturePtr             s1x1WhiteTexture;
    static ppx::grfx::SamplerPtr             sClampedSampler;
    static ppx::grfx::DescriptorSetLayoutPtr sMaterialResourcesLayout;
    static ppx::grfx::DescriptorSetLayoutPtr sMaterialDataLayout;

    static Material sRustedIron;
    static Material sPaintedMetal;
    static Material sCopper;
    static Material sGold;
    static Material sTitanium;
    static Material sZinc;
    static Material sWhiteRoughPlastic;
    static Material sStoneTile;
};

#endif // MATERIAL_H
