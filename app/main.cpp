#include <iostream>
#include <filesystem>
#include <vector>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>
#include <msquic.h>

#include "oatpp/core/base/Environment.hpp"


#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

import IService;

namespace fs = std::filesystem;

// Type alias for the factory function
using CreateServiceFunc = IService * (*)();
using GetDeleterFunc = void(*)(IService*);

std::vector<std::string> find_service_libraries(const std::string& directory) {
    std::vector<std::string> libraries;

    std::cout << "Searching for libraries in: " << directory << "\n";

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            const std::string filename = entry.path().filename().string();
#ifdef _WIN32
            if (filename.ends_with(".dll")) {
#else
            if (filename.ends_with(".so")) {
#endif
                libraries.push_back(entry.path().string());
                std::cout << "Found library: " << entry.path().string() << "\n";
            }
            }
        }

    return libraries;
    }

void load_and_validate_services(const std::string & directory) {
    auto libraries = find_service_libraries(directory);
    if (libraries.empty()) {
        std::cerr << "No service libraries found in directory: " << directory << "\n";
        return;
    }

    for (const auto& lib_path : libraries) {
        std::cout << "Loading library: " << lib_path << "\n";
#ifdef _WIN32
        HMODULE lib_handle = LoadLibraryA(lib_path.c_str());
        if (!lib_handle) {
            std::cerr << "Failed to load library: " << lib_path << "\n";
            continue;
        }

        auto create_service = reinterpret_cast<CreateServiceFunc>(GetProcAddress(lib_handle, "create_service"));
#else
        void* lib_handle = dlopen(lib_path.c_str(), RTLD_LAZY);
        if (!lib_handle) {
            std::cerr << "Failed to load library: " << lib_path << "\n";
            continue;
        }

        auto create_service = reinterpret_cast<CreateServiceFunc>(dlsym(lib_handle, "create_service"));
#endif
        if (!create_service) {
            std::cerr << "Library does not provide create_service function: " << lib_path << "\n";
#ifdef _WIN32
            FreeLibrary(lib_handle);
#else
            dlclose(lib_handle);
#endif
            continue;
        }

        // Create and validate the service instance
        auto service = create_service();
        if (!service) {
            std::cerr << "Failed to create service from library: " << lib_path << "\n";
#ifdef _WIN32
            FreeLibrary(lib_handle);
#else
            dlclose(lib_handle);
#endif
            continue;
        }

        std::cout << "Executing service from: " << lib_path << "\n";
        service->execute();

#ifdef _WIN32
        FreeLibrary(lib_handle);
#else
        dlclose(lib_handle);
#endif
    }
}


void TraceMsQuicVersion() {
    const QUIC_API_TABLE* QuicApi;
    QUIC_STATUS Status = MsQuicOpen2(&QuicApi);
    if (QUIC_FAILED(Status)) {
        std::cerr << "Failed to open MsQuic API: " << Status << std::endl;
        return;
    }

    // Query the library version
    uint32_t Version[4] = { 0 };
    uint32_t BufferLength = sizeof(Version);
    Status = QuicApi->GetParam(
        nullptr,  // Global parameter
        QUIC_PARAM_GLOBAL_LIBRARY_VERSION,
        &BufferLength,
        Version);
    if (QUIC_SUCCEEDED(Status)) {
        std::cout << "MsQuic Library Version: "
            << Version[0] << "." << Version[1] << "." << Version[2]
            << " (Build " << Version[3] << ")" << std::endl;
    }
    else {
        std::cerr << "Failed to query library version: " << Status << std::endl;
    }

    // Query the Git hash
    char GitHash[64] = { 0 };
    BufferLength = sizeof(GitHash);
    Status = QuicApi->GetParam(
        nullptr,  // Global parameter
        QUIC_PARAM_GLOBAL_LIBRARY_GIT_HASH,
        &BufferLength,
        GitHash);
    if (QUIC_SUCCEEDED(Status)) {
        std::cout << "MsQuic Git Hash: " << GitHash << std::endl;
    }
    else {
        std::cerr << "Failed to query Git hash: " << Status << std::endl;
    }

    // Close the API using MsQuicClose
    MsQuicClose(QuicApi);
}

int main() {

    std::cout << "JSON:" 
        << NLOHMANN_JSON_VERSION_MAJOR << "." 
        << NLOHMANN_JSON_VERSION_MINOR << "."
        << NLOHMANN_JSON_VERSION_PATCH << "\n";

    TraceMsQuicVersion();

    std::cout << "oat++ version: " << OATPP_VERSION << std::endl;

    // Get the directory containing app.exe
    const std::string exe_path = fs::current_path().string();
    std::cout << "Executable path: " << exe_path << "\n";

    // Move one level up to search for service libraries
    const std::string parent_dir = fs::path(exe_path).parent_path().string();
    std::cout << "Parent directory: " << parent_dir << "\n";

    // Search for DLLs in all subdirectories of the parent directory
    load_and_validate_services(parent_dir);

    return 0;
}
