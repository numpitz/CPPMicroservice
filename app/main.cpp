#include <iostream>
#include <filesystem>
#include <vector>
#include <memory>
#include <string>

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

int main() {
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
