#include "ppx/grfx/dx/dx_instance.h"
#include "ppx/grfx/dx/dx_device.h"
#include "ppx/grfx/dx/dx_gpu.h"
#include "ppx/grfx/dx/dx_swapchain.h"

#include <dxgidebug.h>

namespace ppx {
namespace grfx {
namespace dx {

Result Instance::EnumerateAndCreateGpus(D3D_FEATURE_LEVEL featureLevel)
{
    // Enumerate GPUs
    std::vector<ComPtr<IDXGIAdapter>> adapters;
    for (UINT index = 0;; ++index) {
        ComPtr<IDXGIAdapter1> adapter;
        // We're done if anything other than S_OK is returned
        HRESULT hr = mFactory->EnumAdapters1(index, &adapter);
        if (hr == DXGI_ERROR_NOT_FOUND) {
            break;
        }
        // Filter for only hardware adapters
        DXGI_ADAPTER_DESC1 desc;
        hr = adapter->GetDesc1(&desc);
        if (FAILED(hr) || (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) {
            continue;
        }
        // Store adapters that support the minimum feature level
        hr = D3D12CreateDevice(adapter.Get(), featureLevel, _uuidof(ID3D12Device), nullptr);
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
        case grfx::API_DX_12_0: featureLevel = D3D_FEATURE_LEVEL_12_0; break;
        case grfx::API_DX_12_1: featureLevel = D3D_FEATURE_LEVEL_12_1; break;
    }
    if (featureLevel == ppx::InvalidValue<D3D_FEATURE_LEVEL>()) {
        return ppx::ERROR_UNSUPPORTED_API;
    }

    UINT dxgiFactoryFlags = 0;
    if (pCreateInfo->enableDebug) {
        // Get debug interface
        ComPtr<ID3D12Debug> debugController;
        HRESULT             hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        if (FAILED(hr)) {
            return ppx::ERROR_API_FAILURE;
        }
        // Enable additional debug layers
        debugController->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

        ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf())))) {
            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
        }
    }

    ComPtr<IDXGIFactory4> dxgiFactory;
    // Create factory using IDXGIFactory4
    HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }
    // Cast to our version of IDXGIFactory
    hr = dxgiFactory.As(&mFactory);
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }

    Result ppxres = EnumerateAndCreateGpus(featureLevel);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void Instance::DestroyApiObjects()
{
    if (mFactory) {
        mFactory.Reset();
    }
}

Result Instance::AllocateObject(grfx::Device** ppDevice)
{
    dx::Device* pObject = new dx::Device();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppDevice = pObject;
    return ppx::SUCCESS;
}

Result Instance::AllocateObject(grfx::Gpu** ppGpu)
{
    dx::Gpu* pObject = new dx::Gpu();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppGpu = pObject;
    return ppx::SUCCESS;
}

Result Instance::AllocateObject(grfx::Surface** ppSurface)
{
    dx::Surface* pObject = new dx::Surface();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppSurface = pObject;
    return ppx::SUCCESS;
}

} // namespace dx
} // namespace grfx
} // namespace ppx
