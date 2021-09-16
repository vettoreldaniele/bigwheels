#include <iostream>
#include <string>

#include "d3dcompile_helper.h"

using namespace ppx;

static std::string LoadHlslFile(const fs::path& baseDir, const std::string& baseName)
{
    fs::path filePath = baseDir;
    filePath          = (filePath / baseName).append_extension(".hlsl");
    if (!fs::exists(filePath)) {
        PPX_ASSERT_MSG(false, "HLSL file not found: " << filePath);
    }
    std::vector<char> hlslCode = fs::load_file(filePath);
    return std::string(hlslCode.data(), hlslCode.size());
}

static const char* EntryPoint(const char* shaderModel)
{
    switch (shaderModel[0]) {
        case 'v':
            return "vsmain";
        case 'p':
            return "psmain";
        case 'c':
            return "csmain";
    }
    return nullptr;
}

std::vector<char> CompileShader(const fs::path& baseDir, const std::string& baseName, const char* shaderModel, ShaderIncludeHandler* includeHandler)
{
    D3D_SHADER_MACRO defines[2] = {
        {"PPX_D3D11", "1"},
        {nullptr, nullptr}};
    ID3DBlob* spirv        = nullptr;
    ID3DBlob* errorMessage = nullptr;

    auto hlslCode = LoadHlslFile(baseDir, baseName);

    HRESULT hr = D3DCompile(reinterpret_cast<LPCVOID>(hlslCode.data()), hlslCode.size(), baseName.data(), defines, includeHandler, EntryPoint(shaderModel), shaderModel, D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &spirv, &errorMessage);
    if (errorMessage) {
        std::cerr << static_cast<char*>(errorMessage->GetBufferPointer())
                  << std::endl;
        PPX_ASSERT_MSG(false, "D3DCompile failed");
    }

    PPX_ASSERT_MSG(hr == S_OK, "D3DCompile failed");

    std::vector<char> spirvCode;
    spirvCode.resize(spirv->GetBufferSize());
    memcpy(spirvCode.data(), spirv->GetBufferPointer(), spirvCode.size());
    return spirvCode;
}

