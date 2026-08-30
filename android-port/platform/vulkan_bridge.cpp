// SDL and RT64 must dispatch through the same Vulkan loader, including surface creation.
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <atomic>

namespace { std::atomic<PFN_vkGetInstanceProcAddr> selected_loader{nullptr}; }

extern "C" __attribute__((visibility("default"))) void dk64_set_vulkan_loader(PFN_vkGetInstanceProcAddr loader) {
    selected_loader.store(loader, std::memory_order_release);
}

extern "C" __attribute__((visibility("default"))) PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance instance, const char* name) {
    auto loader = selected_loader.load(std::memory_order_acquire);
    return loader ? loader(instance, name) : nullptr;
}
