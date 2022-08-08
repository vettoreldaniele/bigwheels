#ifndef ppx_grfx_vk_instance_h
#define ppx_grfx_vk_instance_h

#include "ppx/grfx/vk/000_vk_config.h"
#include "ppx/grfx/grfx_instance.h"

namespace ppx {
namespace grfx {
namespace vk {

class Instance
    : public grfx::Instance
{
public:
    Instance() {}
    virtual ~Instance() {}

    VkInstancePtr GetVkInstance() const { return mInstance; }

protected:
    virtual Result AllocateObject(grfx::Device** ppDevice) override;
    virtual Result AllocateObject(grfx::Gpu** ppGpu) override;
    virtual Result AllocateObject(grfx::Surface** ppSurface) override;

protected:   
    virtual Result CreateApiObjects(const grfx::InstanceCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    Result ConfigureLayersAndExtensions(const grfx::InstanceCreateInfo* pCreateInfo);
    Result CreateDebugUtils(const grfx::InstanceCreateInfo* pCreateInfo);
    Result EnumerateAndCreateeGpus();

private:
    std::vector<std::string> mFoundLayers;
    std::vector<std::string> mFoundExtensions;
    std::vector<std::string> mLayers;
    std::vector<std::string> mExtensions;
    VkInstancePtr            mInstance;
    VkDebugUtilsMessengerPtr mMessenger;

    PFN_vkCreateDebugUtilsMessengerEXT  mFnCreateDebugUtilsMessengerEXT  = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT mFnDestroyDebugUtilsMessengerEXT = nullptr;
    PFN_vkSubmitDebugUtilsMessageEXT    mFnSubmitDebugUtilsMessageEXT    = nullptr;
};

} // namespace vk
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_vk_instance_h
