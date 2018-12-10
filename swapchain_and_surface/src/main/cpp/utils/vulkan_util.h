//
// Created by glumes on 2018/12/6.
//

#ifndef VULKAN_TUTORIAL_VULKAN_UTIL_H
#define VULKAN_TUTORIAL_VULKAN_UTIL_H


#include <vulkan_wrapper.h>
#include <vulkan/vulkan.h>
#include <logutil.h>

#include <vector>

#define APP_SHORT_NAME "VulkanTutorial"


#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
    {                                                                          \
        info.fp##entrypoint =                                                  \
            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
        if (info.fp##entrypoint == NULL) {                                     \
        }                                                                      \
    }


using namespace std;

struct vulkan_tutorial_info {
    VkInstance instance;
    VkDevice device;
    VkQueue queue;
    uint32_t gpu_size;
    vector<VkPhysicalDevice> gpu_physical_devices;

    // physical device 对应的 queue 数量
    uint32_t queue_family_size;
    // queue 的 属性, 有多少个 queue 对应多少个 properties
    vector<VkQueueFamilyProperties> queue_family_props;

    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;

    VkSurfaceKHR surface;

    PFN_vkCreateAndroidSurfaceKHR fpCreateAndroidSurfaceKHR;

    VkFormat format;

    int width, height;

    VkSwapchainKHR swap_chain;

    uint32_t swapchainImageCount;

};


VkResult initVulkan();

void vulkan_init_layer_and_extension_properties(vulkan_tutorial_info &info);

void vulkan_init_instance(struct vulkan_tutorial_info &info);

void vulkan_init_enumerate_device(struct vulkan_tutorial_info &info);

void vulkan_init_queue_family_and_index(struct vulkan_tutorial_info &info);

void vulkan_init_device(struct vulkan_tutorial_info &info);

void vulkan_destroy_device(struct vulkan_tutorial_info &info);

void vulkan_destroy_instance(struct vulkan_tutorial_info &info);


void ErrorCheck(VkResult result);


#endif //VULKAN_TUTORIAL_VULKAN_UTIL_H
