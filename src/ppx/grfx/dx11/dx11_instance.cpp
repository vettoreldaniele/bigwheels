#include "ppx/grfx/dx11/dx11_instance.h"
#include "ppx/grfx/dx11/dx11_device.h"
#include "ppx/grfx/dx11/dx11_gpu.h"
#include "ppx/grfx/dx11/dx11_swapchain.h"

namespace ppx {
namespace grfx {
namespace dx11 {

Result Instance::EnumerateAndCreateGpus(D3D_FEATURE_LEVEL featureLevel, bool enableDebug)
{
    // Enumerate GPUs
    std::vector<ComPtr<IDXGIAdapter1>> adapters;
    for (UINT index = 0;; ++index) {
        ComPtr<IDXGIAdapter1> adapter;
        // We're done if anything other than S_OK is returned
        HRESULT hr = mFactory->EnumAdapters1(index, &adapter);
        if (hr == DXGI_ERROR_NOT_FOUND) {
            break;
        }
        // Filter for only hardware adapters, unless
        // a software renderer is requested.
        DXGI_ADAPTER_DESC1 desc;
        hr = adapter->GetDesc1(&desc);
        bool is_software_adapter = desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
        if (FAILED(hr) || (mCreateInfo.useSoftwareRenderer != is_software_adapter)) {
            continue;
        }
        // Store adapters that support the minimum feature level
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        if (enableDebug) {
            flags |= D3D11_CREATE_DEVICE_DEBUG;
        }
        //
        // When creating a device from an existing adapter (i.e. pAdapter is non-NULL), DriverType must be D3D_DRIVER_TYPE_UNKNOWN.
        //
        hr = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, &featureLevel, 1, D3D11_SDK_VERSION, nullptr, nullptr, nullptr);
        if (SUCCEEDED(hr)) {
            adapters.push_back(adapter);
        }
    }

    // Bail if no GPUs are found
    if (adapters.empty()) {
        return ppx::ERROR_NO_GPUS_FOUND;
    }

    // Create GPUs
    for (size_t i = 0; i < adapters.size(); ++i) {
        DXGI_ADAPTER_DESC adapterDesc = {};
        HRESULT           hr          = adapters[i]->GetDesc(&adapterDesc);
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "Failed to get GPU description");
            return ppx::ERROR_API_FAILURE;
        }

        std::string name;
        // Name - hack convert name from wchar to char
        for (size_t i = 0;; ++i) {
            if (adapterDesc.Description[i] == 0) {
                break;
            }
            char c = static_cast<char>(adapterDesc.Description[i]);
            name.push_back(c);
        }

        grfx::internal::GpuCreateInfo gpuCreateInfo = {};
        gpuCreateInfo.featureLevel                  = static_cast<int32_t>(featureLevel);
        gpuCreateInfo.pApiObject                    = static_cast<void*>(adapters[i].Get());

        grfx::GpuPtr tmpGpu;
        Result       ppxres = CreateGpu(&gpuCreateInfo, &tmpGpu);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "Failed creating GPU object using " << name);
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

Result Instance::CreateApiObjects(const grfx::InstanceCreateInfo* pCreateInfo)
{
    D3D_FEATURE_LEVEL featureLevel = ppx::InvalidValue<D3D_FEATURE_LEVEL>();
    switch (pCreateInfo->api) {
        default: break;
        case grfx::API_DX_11_0: featureLevel = D3D_FEATURE_LEVEL_11_0; break;
        case grfx::API_DX_11_1: featureLevel = D3D_FEATURE_LEVEL_11_1; break;
    }
    if (featureLevel == ppx::InvalidValue<D3D_FEATURE_LEVEL>()) {
        return ppx::ERROR_UNSUPPORTED_API;
    }

    UINT dxgiFactoryFlags = 0;
#if !defined(PPX_DXVK)
    if (pCreateInfo->enableDebug) {
        // Get DXGI debug interface
        HRESULT hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&mDXGIDebug));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "DXGIGetDebugInterface1(DXGIDebug) failed");
            return ppx::ERROR_API_FAILURE;
        }

        // Get DXGI info queue
        hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&mDXGIInfoQueue));
        if (FAILED(hr)) {
            PPX_ASSERT_MSG(false, "DXGIGetDebugInterface1(DXGIInfoQueue) failed");
            return ppx::ERROR_API_FAILURE;
        }

        // Set breaks
        mDXGIInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
        mDXGIInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
    }
#endif // !defined(PPX_DXVK)

#if defined(PPX_DXVK)
    IDXGIFactory7* pFactory = nullptr;
    HRESULT        hr       = CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory7), reinterpret_cast<void**>(&pFactory));
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }
    mFactory = pFactory;
    PPX_LOG_OBJECT_CREATION(DXGIFactory, pFactory);
#else
    ComPtr<IDXGIFactory4> dxgiFactory;
    // Create factory using IDXGIFactory4
    HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }
    PPX_LOG_OBJECT_CREATION(DXGIFactory, dxgiFactory.Get());

    // Cast to our version of IDXGIFactory
    hr = dxgiFactory.As(&mFactory);
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }
#endif

    Result ppxres = EnumerateAndCreateGpus(featureLevel, pCreateInfo->enableDebug);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void Instance::DestroyApiObjects()
{
#if !defined(PPX_DXVK)
    if (mCreateInfo.enableDebug) {
        mDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL));
    }
#endif // !defined(PPX_DXVK)

    if (mFactory) {
        mFactory.Reset();
    }

#if !defined(PPX_DXVK)
    if (mDXGIInfoQueue) {
        mDXGIInfoQueue.Reset();
    }

    if (mDXGIDebug) {
        mDXGIDebug.Reset();
    }
#endif // !defined(PPX_DXVK)
}

Result Instance::AllocateObject(grfx::Device** ppDevice)
{
    dx11::Device* pObject = new dx11::Device();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppDevice = pObject;
    return ppx::SUCCESS;
}

Result Instance::AllocateObject(grfx::Gpu** ppGpu)
{
    dx11::Gpu* pObject = new dx11::Gpu();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppGpu = pObject;
    return ppx::SUCCESS;
}

Result Instance::AllocateObject(grfx::Surface** ppSurface)
{
    dx11::Surface* pObject = new dx11::Surface();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppSurface = pObject;
    return ppx::SUCCESS;
}

} // namespace dx11
} // namespace grfx
} // namespace ppx
