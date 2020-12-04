#include "Material.h"
#include "Render.h"

#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_scope.h"
#include "ppx/application.h"
#include "ppx/graphics_util.h"
using namespace ppx;

#include <map>

const float  F0_Generic         = 0.04f;
const float3 F0_MetalTitanium   = float3(0.542f, 0.497f, 0.449f);
const float3 F0_MetalChromium   = float3(0.549f, 0.556f, 0.554f);
const float3 F0_MetalIron       = float3(0.562f, 0.565f, 0.578f);
const float3 F0_MetalNickel     = float3(0.660f, 0.609f, 0.526f);
const float3 F0_MetalPlatinum   = float3(0.673f, 0.637f, 0.585f);
const float3 F0_MetalCopper     = float3(0.955f, 0.638f, 0.538f);
const float3 F0_MetalPalladium  = float3(0.733f, 0.697f, 0.652f);
const float3 F0_MetalZinc       = float3(0.664f, 0.824f, 0.850f);
const float3 F0_MetalGold       = float3(1.022f, 0.782f, 0.344f);
const float3 F0_MetalAluminum   = float3(0.913f, 0.922f, 0.924f);
const float3 F0_MetalSilver     = float3(0.972f, 0.960f, 0.915f);
const float3 F0_DiletricWater   = float3(0.020f);
const float3 F0_DiletricPlastic = float3(0.040f);
const float3 F0_DiletricGlass   = float3(0.045f);
const float3 F0_DiletricCrystal = float3(0.050f);
const float3 F0_DiletricGem     = float3(0.080f);
const float3 F0_DiletricDiamond = float3(0.150f);

ppx::grfx::TexturePtr             Material::s1x1BlackTexture;
ppx::grfx::TexturePtr             Material::s1x1WhiteTexture;
ppx::grfx::SamplerPtr             Material::sClampedSampler;
ppx::grfx::DescriptorSetLayoutPtr Material::sMaterialResourcesLayout;
ppx::grfx::DescriptorSetLayoutPtr Material::sMaterialDataLayout;

Material Material::sRustedIron;
Material Material::sPaintedMetal;
Material Material::sCopper;
Material Material::sGold;
Material Material::sTitanium;
Material Material::sZinc;
Material Material::sWhiteRoughPlastic;
Material Material::sStoneTile;

static std::map<std::string, grfx::TexturePtr> mTextureCache;

static Result LoadTexture(grfx::Queue* pQueue, const fs::path& path, grfx::Texture** ppTexture)
{
    Result ppxres = ppx::ERROR_FAILED;

    if (!fs::exists(path)) {
        ppx::ERROR_PATH_DOES_NOT_EXIST;
    }

    // Try to load from cache
    std::string normalizedPath = path.c_str();
    auto        it             = mTextureCache.find(normalizedPath);
    if (it != std::end(mTextureCache)) {
        *ppTexture = it->second;
    }
    else {
        TextureCreateOptions textureCreateOptions = TextureCreateOptions().MipLevelCount(PPX_ALL_MIP_LEVELS);
        PPX_CHECKED_CALL(ppxres = CreateTextureFromFile(pQueue, path, ppTexture, textureCreateOptions));
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

ppx::Result Material::Create(ppx::grfx::Queue* pQueue, ppx::grfx::DescriptorPool* pPool, const MaterialCreateInfo* pCreateInfo)
{
    Result ppxres = ppx::ERROR_FAILED;

    grfx::Device* pDevice = pQueue->GetDevice();

    // Material constants temp buffer
    grfx::BufferPtr    tmpCpuMaterialConstants;
    MaterialConstants* pMaterialConstants = nullptr;
    {
        grfx::BufferCreateInfo bufferCreateInfo      = {};
        bufferCreateInfo.size                        = PPX_MINIUM_CONSTANT_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.transferSrc = true;
        bufferCreateInfo.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;
        PPX_CHECKED_CALL(ppxres = pDevice->CreateBuffer(&bufferCreateInfo, &tmpCpuMaterialConstants));

        PPX_CHECKED_CALL(ppxres = tmpCpuMaterialConstants->MapMemory(0, reinterpret_cast<void**>(&pMaterialConstants)));
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Values
    pMaterialConstants->F0          = pCreateInfo->F0;
    pMaterialConstants->albedo      = pCreateInfo->albedo;
    pMaterialConstants->roughness   = pCreateInfo->roughness;
    pMaterialConstants->metalness   = pCreateInfo->metalness;
    pMaterialConstants->iblStrength = pCreateInfo->iblStrength;
    pMaterialConstants->envStrength = pCreateInfo->envStrength;

    // Albedo texture
    mAlbedoTexture = s1x1WhiteTexture;
    if (!pCreateInfo->albedoTexturePath.empty()) {
        PPX_CHECKED_CALL(ppxres = LoadTexture(pQueue, pCreateInfo->albedoTexturePath, &mAlbedoTexture));
        pMaterialConstants->albedoSelect = 1;
    }

    // Roughness texture
    mRoughnessTexture = s1x1BlackTexture;
    if (!pCreateInfo->roughnessTexturePath.empty()) {
        PPX_CHECKED_CALL(ppxres = LoadTexture(pQueue, pCreateInfo->roughnessTexturePath, &mRoughnessTexture));
        pMaterialConstants->roughnessSelect = 1;
    }

    // Metalness texture
    mMetalnessTexture = s1x1BlackTexture;
    if (!pCreateInfo->metalnessTexturePath.empty()) {
        PPX_CHECKED_CALL(ppxres = LoadTexture(pQueue, pCreateInfo->metalnessTexturePath, &mMetalnessTexture));
        pMaterialConstants->metalnessSelect = 1;
    }

    // Normal map texture
    mNormalMapTexture = s1x1BlackTexture;
    if (!pCreateInfo->normalTexturePath.empty()) {
        PPX_CHECKED_CALL(ppxres = LoadTexture(pQueue, pCreateInfo->normalTexturePath, &mNormalMapTexture));
        pMaterialConstants->normalSelect = 1;
    }

    // Unmap since we're done
    tmpCpuMaterialConstants->UnmapMemory();

    // Copy material constants to GPU buffer
    {
        grfx::BufferCreateInfo bufferCreateInfo        = {};
        bufferCreateInfo.size                          = tmpCpuMaterialConstants->GetSize();
        bufferCreateInfo.usageFlags.bits.transferDst   = true;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_GPU_ONLY;
        PPX_CHECKED_CALL(ppxres = pDevice->CreateBuffer(&bufferCreateInfo, &mMaterialConstants));

        grfx::BufferToBufferCopyInfo copyInfo = {tmpCpuMaterialConstants->GetSize()};

        ppxres = pQueue->CopyBufferToBuffer(&copyInfo, tmpCpuMaterialConstants, mMaterialConstants);
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Allocate descriptor sets
    PPX_CHECKED_CALL(ppxres = pDevice->AllocateDescriptorSet(pPool, sMaterialResourcesLayout, &mMaterialResourcesSet));
    PPX_CHECKED_CALL(ppxres = pDevice->AllocateDescriptorSet(pPool, sMaterialDataLayout, &mMaterialDataSet));

    // Update material resource descriptors
    {
        grfx::WriteDescriptor writes[5] = {};
        writes[0].binding               = MATERIAL_ALBEDO_TEXTURE_REGISTER;
        writes[0].arrayIndex            = 0;
        writes[0].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[0].pImageView            = mAlbedoTexture->GetSampledImageView();
        writes[1].binding               = MATERIAL_ROUGHNESS_TEXTURE_REGISTER;
        writes[1].arrayIndex            = 0;
        writes[1].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[1].pImageView            = mRoughnessTexture->GetSampledImageView();
        writes[2].binding               = MATERIAL_METALNESS_TEXTURE_REGISTER;
        writes[2].arrayIndex            = 0;
        writes[2].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[2].pImageView            = mMetalnessTexture->GetSampledImageView();
        writes[3].binding               = MATERIAL_NORMAL_MAP_TEXTURE_REGISTER;
        writes[3].arrayIndex            = 0;
        writes[3].type                  = grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[3].pImageView            = mNormalMapTexture->GetSampledImageView();
        writes[4].binding               = MATERILA_CLAMPED_SAMPLER_REGISTER;
        writes[4].type                  = grfx::DESCRIPTOR_TYPE_SAMPLER;
        writes[4].pSampler              = sClampedSampler;

        PPX_CHECKED_CALL(ppxres = mMaterialResourcesSet->UpdateDescriptors(5, writes));
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    // Update material data descriptors
    {
        grfx::WriteDescriptor write = {};
        write.binding               = MODEL_CONSTANTS_REGISTER;
        write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.bufferOffset          = 0;
        write.bufferRange           = PPX_WHOLE_SIZE;
        write.pBuffer               = mMaterialConstants;
        PPX_CHECKED_CALL(ppxres = mMaterialDataSet->UpdateDescriptors(1, &write));
        if (Failed(ppxres)) {
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

void Material::Destroy()
{
}

ppx::Result Material::CreateMaterials(ppx::grfx::Queue* pQueue, ppx::grfx::DescriptorPool* pPool)
{
    Result ppxres = ppx::ERROR_FAILED;

    grfx::Device* pDevice = pQueue->GetDevice();

    // Create 1x1 black and white textures
    {
        PPX_CHECKED_CALL(ppxres = CreateTexture1x1(pQueue, float4(0), &s1x1BlackTexture));
        PPX_CHECKED_CALL(ppxres = CreateTexture1x1(pQueue, float4(1), &s1x1WhiteTexture));
    }

    // Create sampler
    {
        grfx::SamplerCreateInfo createInfo = {};
        createInfo.magFilter               = grfx::FILTER_LINEAR;
        createInfo.minFilter               = grfx::FILTER_LINEAR;
        createInfo.mipmapMode              = grfx::SAMPLER_MIPMAP_MODE_LINEAR;
        PPX_CHECKED_CALL(ppxres = pDevice->CreateSampler(&createInfo, &sClampedSampler));
    }

    // Material resources layout
    {
        // clang-format off
        grfx::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.bindings.push_back({grfx::DescriptorBinding{MATERIAL_ALBEDO_TEXTURE_REGISTER,     grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{MATERIAL_ROUGHNESS_TEXTURE_REGISTER,  grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{MATERIAL_METALNESS_TEXTURE_REGISTER,  grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{MATERIAL_NORMAL_MAP_TEXTURE_REGISTER, grfx::DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        createInfo.bindings.push_back({grfx::DescriptorBinding{MATERILA_CLAMPED_SAMPLER_REGISTER,    grfx::DESCRIPTOR_TYPE_SAMPLER,       1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        PPX_CHECKED_CALL(ppxres = pDevice->CreateDescriptorSetLayout(&createInfo, &sMaterialResourcesLayout));
        // clang-format on
    }

    // Material data layout
    {
        grfx::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.bindings.push_back({grfx::DescriptorBinding{0, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        PPX_CHECKED_CALL(ppxres = pDevice->CreateDescriptorSetLayout(&createInfo, &sMaterialDataLayout));
    }

    // RustedIron
    {
        MaterialCreateInfo createInfo   = {};
        createInfo.F0                   = F0_Generic;
        createInfo.albedo               = F0_MetalIron;
        createInfo.roughness            = 0;
        createInfo.metalness            = 1;
        createInfo.iblStrength          = 1;
        createInfo.envStrength          = 0.2f;
        createInfo.albedoTexturePath    = Application::Get()->GetAssetPath("materials/textures/RustedIron/albedo.png");
        createInfo.roughnessTexturePath = Application::Get()->GetAssetPath("materials/textures/RustedIron/roughness.png");
        createInfo.metalnessTexturePath = Application::Get()->GetAssetPath("materials/textures/RustedIron/metalness.png");
        createInfo.normalTexturePath    = Application::Get()->GetAssetPath("materials/textures/RustedIron/normal.png");

        PPX_CHECKED_CALL(ppxres = sRustedIron.Create(pQueue, pPool, &createInfo));
    }

    // PaintedMetal
    {
        MaterialCreateInfo createInfo   = {};
        createInfo.F0                   = F0_Generic;
        createInfo.albedo               = F0_MetalCopper;
        createInfo.roughness            = 0;
        createInfo.metalness            = 1;
        createInfo.iblStrength          = 1;
        createInfo.envStrength          = 0.2f;
        createInfo.albedoTexturePath    = Application::Get()->GetAssetPath("materials/textures/PaintedMetal/albedo.jpg");
        createInfo.roughnessTexturePath = Application::Get()->GetAssetPath("materials/textures/PaintedMetal/roughness.jpg");
        createInfo.metalnessTexturePath = Application::Get()->GetAssetPath("materials/textures/PaintedMetal/metalness.jpg");
        createInfo.normalTexturePath    = Application::Get()->GetAssetPath("materials/textures/PaintedMetal/normal.jpg");

        PPX_CHECKED_CALL(ppxres = sPaintedMetal.Create(pQueue, pPool, &createInfo));
    }

    // Copper
    {
        MaterialCreateInfo createInfo = {};
        createInfo.F0                 = F0_Generic;
        createInfo.albedo             = F0_MetalCopper;
        createInfo.roughness          = 0.5f;
        createInfo.metalness          = 1;
        createInfo.iblStrength        = 1;
        createInfo.envStrength        = 0.1f;

        PPX_CHECKED_CALL(ppxres = sCopper.Create(pQueue, pPool, &createInfo));
    }

    // Gold
    {
        MaterialCreateInfo createInfo = {};
        createInfo.F0                 = F0_Generic;
        createInfo.albedo             = F0_MetalGold;
        createInfo.roughness          = 0.25f;
        createInfo.metalness          = 1;
        createInfo.iblStrength        = 1;
        createInfo.envStrength        = 0.3f;

        PPX_CHECKED_CALL(ppxres = sGold.Create(pQueue, pPool, &createInfo));
    }

    // Titanium
    {
        MaterialCreateInfo createInfo = {};
        createInfo.F0                 = F0_Generic;
        createInfo.albedo             = F0_MetalTitanium;
        createInfo.roughness          = 0.25f;
        createInfo.metalness          = 1;
        createInfo.iblStrength        = 1;
        createInfo.envStrength        = 0.3f;

        PPX_CHECKED_CALL(ppxres = sTitanium.Create(pQueue, pPool, &createInfo));
    }

    // Zinc
    {
        MaterialCreateInfo createInfo = {};
        createInfo.F0                 = F0_Generic;
        createInfo.albedo             = F0_MetalZinc;
        createInfo.roughness          = 0.15f;
        createInfo.metalness          = 1;
        createInfo.iblStrength        = 1;
        createInfo.envStrength        = 0.3f;

        PPX_CHECKED_CALL(ppxres = sZinc.Create(pQueue, pPool, &createInfo));
    }

    // White Rough Plastic
    {
        MaterialCreateInfo createInfo = {};
        createInfo.F0                 = F0_DiletricPlastic.x;
        createInfo.albedo             = float3(1.0f, 1.0f, 1.0f);
        createInfo.roughness          = 0.9f;
        createInfo.metalness          = 0;
        createInfo.iblStrength        = 0.025f;
        createInfo.envStrength        = 0.0f;

        PPX_CHECKED_CALL(ppxres = sWhiteRoughPlastic.Create(pQueue, pPool, &createInfo));
    }

    // Stone Tile
    {
        MaterialCreateInfo createInfo   = {};
        createInfo.F0                   = F0_Generic;
        createInfo.albedo               = F0_DiletricPlastic;
        createInfo.roughness            = 0;
        createInfo.metalness            = 0;
        createInfo.iblStrength          = 0.6f;
        createInfo.envStrength          = 0.05f;
        createInfo.albedoTexturePath    = Application::Get()->GetAssetPath("materials/textures/stone-tile4b/albedo.png");
        createInfo.roughnessTexturePath = Application::Get()->GetAssetPath("materials/textures/stone-tile4b/roughness.png");
        //createInfo.metalnessTexturePath = Application::Get()->GetAssetPath("materials/textures/stone-tile4b/metalness.png");
        createInfo.normalTexturePath    = Application::Get()->GetAssetPath("materials/textures/stone-tile4b/normal.png");

        PPX_CHECKED_CALL(ppxres = sStoneTile.Create(pQueue, pPool, &createInfo));
    }

    return ppx::SUCCESS;
}

void Material::DestroyMaterials()
{
}
