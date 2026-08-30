#include "android_driver.h"
#include <SDL.h>
#include <android/log.h>
#include <adrenotools/driver.h>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include "json/json.hpp"
#include "contrib/plume/plume_vulkan.h"

extern "C" void dk64_set_vulkan_loader(PFN_vkGetInstanceProcAddr loader);

namespace {
void* loader_handle = nullptr; // Must outlive every Vulkan object and SDL surface.
PFN_vkGetInstanceProcAddr loader_proc = nullptr;
std::filesystem::path status_path;
nlohmann::json status;

void write_status() {
    std::ofstream out(status_path, std::ios::trunc);
    out << status.dump(2);
    __android_log_print(ANDROID_LOG_INFO, "DK64Driver", "%s", status.dump().c_str());
    fprintf(stdout, "Graphics driver: %s\n", status.dump().c_str());
}
VkResult initialize_volk() {
    if (!loader_proc) return VK_ERROR_INITIALIZATION_FAILED;
    volkInitializeCustom(loader_proc);
    return VK_SUCCESS;
}
void observe_device(const VkPhysicalDeviceProperties& properties) {
    status["device"] = properties.deviceName;
    status["vendorId"] = properties.vendorID;
    status["driverVersion"] = properties.driverVersion;
    write_status();
}
}

bool dk64_android_initialize_graphics(const char* data_path, const char* native_library_dir,
        const char* cache_dir, const char* driver_dir, const char* driver_library, const char* driver_label) {
    status_path = std::filesystem::path(data_path) / "graphics-driver.json";
    status = {{"mode", "system"}, {"label", "System driver"}, {"message", "Using Android system Vulkan"}};
    if (driver_dir[0] && driver_library[0]) {
        const auto imports = std::filesystem::weakly_canonical(std::filesystem::path(data_path).parent_path() / "gpu-drivers/imports");
        const auto library = std::filesystem::weakly_canonical(std::filesystem::path(driver_dir) / driver_library);
        const auto relative = library.lexically_relative(imports);
        if (relative.empty() || relative.is_absolute() || *relative.begin() == ".." || !std::filesystem::is_regular_file(library)) {
            status["message"] = "Invalid custom driver path; using system driver";
        } else {
            const auto directory = library.parent_path().string() + "/";
            loader_handle = adrenotools_open_libvulkan(RTLD_NOW | RTLD_LOCAL, ADRENOTOOLS_DRIVER_CUSTOM,
                cache_dir, native_library_dir, directory.c_str(), driver_library, nullptr, nullptr);
            if (loader_handle) loader_proc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(loader_handle, "vkGetInstanceProcAddr"));
            if (loader_proc) {
                status["mode"] = "custom";
                status["label"] = driver_label;
                status["message"] = "Custom Vulkan loader initialized";
            } else {
                const char* error = dlerror();
                status["message"] = std::string("Custom driver could not load; using system driver. ") + (error ? error : "No Vulkan entry point");
                if (loader_handle) dlclose(loader_handle);
                loader_handle = nullptr;
            }
        }
    }
    if (!loader_proc) {
        loader_handle = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
        if (loader_handle) loader_proc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(loader_handle, "vkGetInstanceProcAddr"));
    }
    if (!loader_proc) {
        status["message"] = "Unable to load Vulkan on this device";
        write_status();
        return false;
    }
    dk64_set_vulkan_loader(loader_proc);
    const auto bridge_path = std::filesystem::path(native_library_dir) / "libdk64vulkan.so";
    SDL_setenv("SDL_VULKAN_LIBRARY", bridge_path.c_str(), 1);
    plume::SetVulkanLoaderInitializeCallback(initialize_volk);
    plume::SetVulkanPhysicalDeviceObserver(observe_device);
    write_status();
    return true;
}
