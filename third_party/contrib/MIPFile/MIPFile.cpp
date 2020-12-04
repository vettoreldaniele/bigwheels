#include "MIPFile.h"

#include <fstream>

const uint32_t kFileSignature = MIP_FILE_SIGNATURE;
const uint32_t kDataSignature = MIP_DATA_SIGNATURE;
const uint32_t kInfoSignature = MIP_INFO_SIGNATURE;

static bool StreamRead(std::istream& is, char* data, size_t size)
{
    if (is.eof()) {
        return false;
    }
    is.read(data, size);
    return true;
}

static void StreamWrite(std::ostream& os, const char* data, size_t size)
{
    os.write(data, size);
}

uint32_t MIPFormatComponentCount(MIPPixelFormat format)
{
    // clang-format off
    switch (format) {
        default: break;
        case MIP_PIXEL_FORMAT_R8_UINT            : return 1; break;
        case MIP_PIXEL_FORMAT_R8G8_UINT          : return 2; break;
        case MIP_PIXEL_FORMAT_R8G8B8_UINT        : return 3; break;
        case MIP_PIXEL_FORMAT_R8G8B8A8_UINT      : return 4; break;
        case MIP_PIXEL_FORMAT_R16_UINT           : return 1; break;
        case MIP_PIXEL_FORMAT_R16G16_UINT        : return 2; break;
        case MIP_PIXEL_FORMAT_R16G16B16_UINT     : return 3; break;
        case MIP_PIXEL_FORMAT_R16G16B16A16_UINT  : return 4; break;
        case MIP_PIXEL_FORMAT_R16_FLOAT          : return 1; break;
        case MIP_PIXEL_FORMAT_R16G16_FLOAT       : return 2; break;
        case MIP_PIXEL_FORMAT_R16G16B16_FLOAT    : return 3; break;
        case MIP_PIXEL_FORMAT_R16G16B16A16_FLOAT : return 4; break;
        case MIP_PIXEL_FORMAT_R32_FLOAT          : return 1; break;
        case MIP_PIXEL_FORMAT_R32G32_FLOAT       : return 2; break;
        case MIP_PIXEL_FORMAT_R32G32B32_FLOAT    : return 3; break;
        case MIP_PIXEL_FORMAT_R32G32B32A32_FLOAT : return 4; break;
    }
    return 0;
    // clang-format on
}

uint32_t MIPFormatComponentSize(MIPPixelFormat format)
{
    // clang-format off
    switch (format) {
        default: break;
        case MIP_PIXEL_FORMAT_R8_UINT            : return 1; break;
        case MIP_PIXEL_FORMAT_R8G8_UINT          : return 1; break;
        case MIP_PIXEL_FORMAT_R8G8B8_UINT        : return 1; break;
        case MIP_PIXEL_FORMAT_R8G8B8A8_UINT      : return 1; break;

        case MIP_PIXEL_FORMAT_R16_UINT           : return 2; break;
        case MIP_PIXEL_FORMAT_R16G16_UINT        : return 2; break;
        case MIP_PIXEL_FORMAT_R16G16B16_UINT     : return 2; break;
        case MIP_PIXEL_FORMAT_R16G16B16A16_UINT  : return 2; break;

        case MIP_PIXEL_FORMAT_R16_FLOAT          : return 2; break;
        case MIP_PIXEL_FORMAT_R16G16_FLOAT       : return 2; break;
        case MIP_PIXEL_FORMAT_R16G16B16_FLOAT    : return 2; break;
        case MIP_PIXEL_FORMAT_R16G16B16A16_FLOAT : return 2; break;

        case MIP_PIXEL_FORMAT_R32_FLOAT          : return 4; break;
        case MIP_PIXEL_FORMAT_R32G32_FLOAT       : return 4; break;
        case MIP_PIXEL_FORMAT_R32G32B32_FLOAT    : return 4; break;
        case MIP_PIXEL_FORMAT_R32G32B32A32_FLOAT : return 4; break;
    }
    return 0;
    // clang-format on
}

uint32_t MIPFormatPixelSize(MIPPixelFormat format)
{
    uint32_t componentCount = MIPFormatComponentCount(format);
    uint32_t componentSize  = MIPFormatComponentSize(format);
    uint32_t pixelSize      = componentCount * componentSize;
    return pixelSize;
}

static bool ReadFileInfo(std::ifstream& is, MIPFileInfo* p_file_info)
{
    if (p_file_info == nullptr) {
        return false;
    }

    // File Signature
    if (!StreamRead(is, reinterpret_cast<char*>(&p_file_info->file_signature), sizeof(kFileSignature))) {
        return false;
    }
    if (p_file_info->file_signature != kFileSignature) {
        return false;
    }

    // Pixel format
    if (!StreamRead(is, reinterpret_cast<char*>(&p_file_info->pixel_format), sizeof(p_file_info->pixel_format))) {
        return false;
    }
    // Level count
    if (!StreamRead(is, reinterpret_cast<char*>(&p_file_info->level_count), sizeof(p_file_info->level_count))) {
        return false;
    }
    // Reserved
    if (!StreamRead(is, reinterpret_cast<char*>(&p_file_info->reserved), sizeof(p_file_info->reserved))) {
        return false;
    }

    // Load MIP info signature
    if (!StreamRead(is, reinterpret_cast<char*>(&p_file_info->level_info_signature), sizeof(kInfoSignature))) {
        return false;
    }

    // Load MIP info for each level
    for (uint32_t level = 0; level < p_file_info->level_count; ++level) {
        MIPLevelInfo* p_info = &p_file_info->level_infos[level];
        // Level
        if (!StreamRead(is, reinterpret_cast<char*>(&p_info->level), sizeof(p_info->level))) {
            return false;
        }
        // Data offset
        if (!StreamRead(is, reinterpret_cast<char*>(&p_info->data_offset), sizeof(p_info->data_offset))) {
            return false;
        }
        // Data size
        if (!StreamRead(is, reinterpret_cast<char*>(&p_info->data_size), sizeof(p_info->data_size))) {
            return false;
        }
        // Width
        if (!StreamRead(is, reinterpret_cast<char*>(&p_info->width), sizeof(p_info->width))) {
            return false;
        }
        // Height
        if (!StreamRead(is, reinterpret_cast<char*>(&p_info->height), sizeof(p_info->height))) {
            return false;
        }
        // Row stride
        if (!StreamRead(is, reinterpret_cast<char*>(&p_info->row_stride), sizeof(p_info->row_stride))) {
            return false;
        }
    }

    // Accumulate total size
    p_file_info->data_size = 0;
    for (size_t i = 0; i < p_file_info->level_count; ++i) {
        p_file_info->data_size += p_file_info->level_infos[i].data_size;
    }

    return true;
}

bool MIPLoadInfo(const char* file_path, MIPFileInfo* p_file_info)
{
    if ((file_path == nullptr) || (p_file_info == nullptr)) {
        return false;
    }

    std::ifstream is(file_path, std::ios::binary);
    if (!is.is_open()) {
        return false;
    }

    bool result = ReadFileInfo(is, p_file_info);
    if (!result) {
        return false;
    }

    return true;
}

bool MIPLoadData(const char* file_path, uint32_t level, uint32_t level_count, char* p_data)
{
    if ((file_path == nullptr) || (p_data == nullptr)) {
        return false;
    }

    if (((level + level_count) >= MAX_MIP_LEVELS) || (level_count == 0)) {
        return false;
    }

    std::ifstream is(file_path, std::ios::binary);
    if (!is.is_open()) {
        return false;
    }

    MIPFileInfo file_info = {};
    bool result = ReadFileInfo(is, &file_info);
    if (!result) {
        return false;
    }

    std::streampos data_offset = static_cast<std::streampos>(file_info.level_infos[level].data_offset);
    is.seekg(data_offset, std::ios::cur);

    // Accumulate data size
    size_t data_size = 0;
    for (uint32_t i = 0; i < level_count; ++i) {
        uint32_t level_index = level + i;
        data_size += file_info.level_infos[level_index].data_size;
    }

    if (!StreamRead(is, p_data, data_size)) {
        return false;
    }

    return true;
}

/*
bool MIPAllocateData(MIPFile* p_mip_file)
{
    if (p_mip_file == nullptr) {
        return false;
    }

    if (p_mip_file->level_count == 0) {
        return false;
    }

    uint64_t total_data_size = 0;
    for (size_t i = 0; i < p_mip_file->level_count; ++i) {
        total_data_size += p_mip_file->infos[i].data_size;
    }

    p_mip_file->p_data = (char*)malloc(p_mip_file->data_size);
}

void MIPFreeData(MIPFile* p_mip_file)
{
    if (p_mip_file->p_data == nullptr) {
        return;
    }

    free(p_mip_file->p_data);
    p_mip_file->p_data = nullptr;
}

bool MIPLoadData(const char* file_path, MIPFile* p_mip_file)
{
    if ((file_path == nullptr) || (p_mip_file == nullptr)) {
        return false;
    }
    
    std::ifstream is(file_path, std::ios::binary);
    if (!is.is_open()) {
        return false;
    }

    // Load header
    bool result = ReadInfo(is, p_mip_file);
    if (!result) {
        return false;
    }
}

bool MIPLoadFile(const char* file_path, MIPFile* p_mip_file)
{
    if ((file_path == nullptr) || (p_mip_file == nullptr)) {
        return false;
    }
    
    std::ifstream is(file_path, std::ios::binary);
    if (!is.is_open()) {
        return false;
    }

    // Load header
    bool result = ReadInfo(is, p_mip_file);
    if (!result) {
        return false;
    }


    // Load data
    {
        p_mip_file->data.resize(total_data_size);

        // Data Signature
        size_t file_offset = is.tellg();
        StreamRead(is, reinterpret_cast<char*>(&p_mip_file->data_signature), sizeof(kDataSignature));
        // Data
        file_offset = is.tellg();
        if (total_data_size > 0) {
            char* data = reinterpret_cast<char*>(p_mip_file->data.data());
            StreamRead(is, data, total_data_size);
        }
    }

    return true;
}
*/

bool MIPWriteFile(const char* file_path, const MIPFile* p_mip_file)
{
    std::ofstream os(file_path, std::ios::binary);
    if (!os.is_open()) {
        return false;
    }

    const MIPFileInfo& file_info = p_mip_file->file_info;

    // File signature
    StreamWrite(os, reinterpret_cast<const char*>(&kFileSignature), sizeof(kFileSignature));
    // Pixel format
    StreamWrite(os, reinterpret_cast<const char*>(&file_info.pixel_format), sizeof(file_info.pixel_format));
    // Level count
    StreamWrite(os, reinterpret_cast<const char*>(&file_info.level_count), sizeof(file_info.level_count));
    // Reserved
    StreamWrite(os, reinterpret_cast<const char*>(&file_info.reserved), sizeof(file_info.reserved));

    // Write MIP infos
    {
        // Write MIP info signature
        StreamWrite(os, reinterpret_cast<const char*>(&kInfoSignature), sizeof(kInfoSignature));

        // Write MIP info for each level
        for (uint32_t level = 0; level < file_info.level_count; ++level) {
            const MIPLevelInfo* p_info = &file_info.level_infos[level];
            StreamWrite(os, reinterpret_cast<const char*>(&p_info->level), sizeof(p_info->level));
            StreamWrite(os, reinterpret_cast<const char*>(&p_info->data_offset), sizeof(p_info->data_offset));
            StreamWrite(os, reinterpret_cast<const char*>(&p_info->data_size), sizeof(p_info->data_size));
            StreamWrite(os, reinterpret_cast<const char*>(&p_info->width), sizeof(p_info->width));
            StreamWrite(os, reinterpret_cast<const char*>(&p_info->height), sizeof(p_info->height));
            StreamWrite(os, reinterpret_cast<const char*>(&p_info->row_stride), sizeof(p_info->row_stride));
        }
    }

    // Write data
    {
        // Data signature
        size_t file_offset = os.tellp();
        StreamWrite(os, reinterpret_cast<const char*>(&kDataSignature), sizeof(kDataSignature));
        // Data size
        const uint64_t data_size = static_cast<uint64_t>(file_info.data_size);
        file_offset              = os.tellp();
        // Data
        if (data_size > 0) {
            const char* data = reinterpret_cast<const char*>(p_mip_file->p_data);
            StreamWrite(os, data, data_size);
        }
    }

    os.close();

    return true;
}
