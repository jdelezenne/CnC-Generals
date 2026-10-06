#include "ShaderCompiler.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <dxcapi.h>
#include <memory>

namespace {
template<class T> struct Release {
    void operator()(T* object) const { if (object) object->Release(); }
};
template<class T> using Com = std::unique_ptr<T, Release<T>>;
}

bool Platform::GPU::CompileHLSL(const std::string& source, bool vertex, bool spirv,
    std::vector<std::uint8_t>& bytecode, std::string& error)
{
    bytecode.clear();
    IDxcCompiler3* rawCompiler = nullptr;
    if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3),
        reinterpret_cast<void**>(&rawCompiler)))) {
        error = "Cannot initialize DXC";
        return false;
    }
    Com<IDxcCompiler3> compiler(rawCompiler);
    std::vector<LPCWSTR> arguments{L"-E", L"main", L"-T", vertex ? L"vs_6_0" : L"ps_6_0",
        L"-O3", L"-Zpr", L"-Gis"};
    if (spirv) {
        arguments.insert(arguments.end(), {L"-spirv", L"-fspv-target-env=vulkan1.0", L"-fvk-use-dx-layout"});
    }
    DxcBuffer buffer{source.data(), source.size(), DXC_CP_UTF8};
    IDxcResult* rawResult = nullptr;
    HRESULT status = compiler->Compile(&buffer, arguments.data(), static_cast<UINT32>(arguments.size()),
        nullptr, __uuidof(IDxcResult), reinterpret_cast<void**>(&rawResult));
    if (FAILED(status)) {
        error = "DXC compile call failed: " + std::to_string(static_cast<unsigned>(status));
        return false;
    }
    Com<IDxcResult> result(rawResult);
    result->GetStatus(&status);
    if (FAILED(status)) {
        IDxcBlobUtf8* rawErrors = nullptr;
        result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), reinterpret_cast<void**>(&rawErrors), nullptr);
        Com<IDxcBlobUtf8> errors(rawErrors);
        error = errors ? errors->GetStringPointer() : "Unknown shader compilation error";
        return false;
    }
    IDxcBlob* rawObject = nullptr;
    result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), reinterpret_cast<void**>(&rawObject), nullptr);
    Com<IDxcBlob> object(rawObject);
    if (!object) {
        error = "DXC returned no shader object";
        return false;
    }
    const auto* bytes = static_cast<const std::uint8_t*>(object->GetBufferPointer());
    bytecode.assign(bytes, bytes + object->GetBufferSize());
    return true;
}
