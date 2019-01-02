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

using namespace std;




struct vulkan_tutorial_info {
    VkInstance instance;
    VkDevice device;
    VkQueue queue;
    uint32_t gpu_size;
    vector<VkPhysicalDevice> gpu_physical_devices;

    uint32_t queue_family_size;
    vector<VkQueueFamilyProperties> queue_family_props;

    uint32_t graphics_queue_family_index;

    std::vector<VkDescriptorSetLayout> desc_layout;

    VkPipelineLayout pipeline_layout;

    VkDescriptorPool desc_pool;

    std::vector<VkDescriptorSet> desc_set;


    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } uniform_data;
};


VkResult initVulkan();

void vulkan_init_instance(struct vulkan_tutorial_info &info);

void vulkan_init_enumerate_device(struct vulkan_tutorial_info &info);

void vulkan_init_queue_family_and_index(struct vulkan_tutorial_info &info);

void vulkan_init_device(struct vulkan_tutorial_info &info);

void vulkan_destroy_device(struct vulkan_tutorial_info &info);

void vulkan_destroy_instance(struct vulkan_tutorial_info &info);


void ErrorCheck(VkResult result);


#endif //VULKAN_TUTORIAL_VULKAN_UTIL_H
