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

typedef struct _swap_chain_buffers {
    VkImage image;
    VkImageView view;
} swap_chain_buffer;


struct vulkan_tutorial_info {
    VkInstance instance;
    VkDevice device;
    VkQueue queue;
    uint32_t gpu_size;
    vector<VkPhysicalDevice> gpu_physical_devices;

    // instance 的 layer 属性
    std::vector<const char *> instance_layer_names;
    // instance 的 extension 拓展
    std::vector<const char *> instance_extension_names;
    // device 的 extension 拓展
    std::vector<const char *> device_extension_names;

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

    std::vector<swap_chain_buffer> buffers;

};


VkResult initVulkan();


void vulkan_init_instance_extension_name(struct vulkan_tutorial_info &info);

void vulkan_init_device_extension_name(struct vulkan_tutorial_info &info);

void vulkan_init_instance(struct vulkan_tutorial_info &info);

void vulkan_init_enumerate_device(struct vulkan_tutorial_info &info);

void vulkan_init_queue_family_and_index(struct vulkan_tutorial_info &info);

void vulkan_init_device(struct vulkan_tutorial_info &info);

void vulkan_init_swapchain(struct vulkan_tutorial_info &info);

void vulkan_destroy_device(struct vulkan_tutorial_info &info);

void vulkan_destroy_instance(struct vulkan_tutorial_info &info);


void ErrorCheck(VkResult result);


#endif //VULKAN_TUTORIAL_VULKAN_UTIL_H
