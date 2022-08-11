#include "ppx/grfx/dx12/dx12_instance.h"
#include "ppx/grfx/dx12/dx12_device.h"
#include "ppx/grfx/dx12/dx12_gpu.h"
#include "ppx/grfx/dx12/dx12_swapchain.h"

namespace ppx {
namespace grfx {
namespace dx12 {

Result Instance::EnumerateAndCreateGpus(D3D_FEATURE_LEVEL featureLevel)
{
    // Enumerate GPUs
    std::vector<CComPtr<IDXGIAdapter1>> adapters;
    for (UINT index = 0;; ++index) {
        CComPtr<IDXGIAdapter1> adapter;
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

        // On GGP we cannot call D3D12CreateDevice here. That will cause
        // it to be called twice, once here under the call frame of
        // grfx::CreateInstance and again later under grfx::CreateDevice.
        // That breaks because libggp expects it to be called only once;
        // the first time it's called it creates some resources with the
        // device, and if that first device is destroyed and then later
        // recreated, future uses of the new device won't be compatible
        // with resources created on the old device.
#ifndef __ggp__
        // Store adapters that support the minimum feature level
        hr = D3D12CreateDevice(adapter.Get(), featureLevel, __uuidof(ID3D12Device), nullptr);
#endif

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
#if !defined(PPX_DXIIVK)
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

        // Get D3D12 debug interface
        hr = D3D12GetDebugInterface(IID_PPV_ARGS(&mD3D12Debug));
        if (FAILED(hr)) {
            return ppx::ERROR_API_FAILURE;
        }
        // Enable additional debug layers
        mD3D12Debug->EnableDebugLayer();
#endif // ! defined (PPX_DXIIVK)
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#if defined(PPX_DXIIVK)
    IDXGIFactory7* pFactory = nullptr;
    HRESULT        hr       = CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(IDXGIFactory7), reinterpret_cast<void**>(&pFactory));
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }
    mFactory = pFactory;
    PPX_LOG_OBJECT_CREATION(DXGIFactory, pFactory);
#else
    CComPtr<IDXGIFactory4> dxgiFactory;
    // Create factory using IDXGIFactory4
    HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }
    PPX_LOG_OBJECT_CREATION(DXGIFactory, dxgiFactory.Get());

    // Cast to our version of IDXGIFactory
    hr = dxgiFactory.QueryInterface(&mFactory);
    if (FAILED(hr)) {
        return ppx::ERROR_API_FAILURE;
    }
#endif // defined(PPX_DXIIVK)

    Result ppxres = EnumerateAndCreateGpus(featureLevel);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void Instance::DestroyApiObjects()
{
#if !defined(PPX_DXIIVK)
    if (mCreateInfo.enableDebug) {
        mDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL));
    }
#endif // ! defined (PPX_DXIIVK)

    if (mFactory) {
        mFactory.Reset();
    }

#if !defined(PPX_DXIIVK)
    if (mD3D12Debug) {
        mD3D12Debug.Reset();
    }

    if (mDXGIInfoQueue) {
        mDXGIInfoQueue.Reset();
    }

    if (mDXGIDebug) {
        mDXGIDebug.Reset();
    }
#endif // ! defined (PPX_DXIIVK)
}

Result Instance::AllocateObject(grfx::Device** ppDevice)
{
    dx12::Device* pObject = new dx12::Device();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppDevice = pObject;
    return ppx::SUCCESS;
}

Result Instance::AllocateObject(grfx::Gpu** ppGpu)
{
    dx12::Gpu* pObject = new dx12::Gpu();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppGpu = pObject;
    return ppx::SUCCESS;
}

Result Instance::AllocateObject(grfx::Surface** ppSurface)
{
    dx12::Surface* pObject = new dx12::Surface();
    if (IsNull(pObject)) {
        return ppx::ERROR_ALLOCATION_FAILED;
    }
    *ppSurface = pObject;
    return ppx::SUCCESS;
}

} // namespace dx12
} // namespace grfx
} // namespace ppx
