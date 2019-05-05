//
// Created by glumes on 2018/12/6.
//

#ifndef VULKAN_TUTORIAL_VULKAN_UTIL_H
#define VULKAN_TUTORIAL_VULKAN_UTIL_H


#include <vulkan_wrapper.h>
#include <vulkan/vulkan.h>
#include <logutil.h>
#include <unistd.h>

#include <vector>

#define APP_SHORT_NAME "VulkanTutorial"

#define FENCE_TIMEOUT 100000000

#define NUM_DESCRIPTOR_SETS 1

#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

#include "shaderc/shaderc.hpp"

#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS


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


// shader
struct shader_type_mapping {
    VkShaderStageFlagBits vkshader_type;
    shaderc_shader_kind shaderc_type;
};

static const shader_type_mapping shader_map_table[] = {
        {VK_SHADER_STAGE_VERTEX_BIT, shaderc_glsl_vertex_shader},
        {VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, shaderc_glsl_tess_control_shader},
        {VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, shaderc_glsl_tess_evaluation_shader},
        {VK_SHADER_STAGE_GEOMETRY_BIT, shaderc_glsl_geometry_shader},
        {VK_SHADER_STAGE_FRAGMENT_BIT, shaderc_glsl_fragment_shader},
        {VK_SHADER_STAGE_COMPUTE_BIT, shaderc_glsl_compute_shader},
};

shaderc_shader_kind MapShadercType(VkShaderStageFlagBits vkShader) {
    for (auto shader : shader_map_table) {
        if (shader.vkshader_type == vkShader) {
            return shader.shaderc_type;
        }
    }
    assert(false);
    return shaderc_glsl_infer_from_source;
}

bool memory_type_from_properties(struct sample_info &info, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
    for (uint32_t i = 0; i < info.memory_properties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((info.memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}

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

    uint32_t current_buffer;

    VkRenderPass render_pass;


    struct {
        VkFormat format;

        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;

    VkFramebuffer *framebuffers;


    VkCommandBuffer cmd; // Buffer for initialization commands

    // 之前没有
    VkQueue graphics_queue;
    VkQueue present_queue;

    VkCommandPool cmd_pool;

    // pipeline
    std::vector<VkDescriptorSetLayout> desc_layout;
    VkPipelineLayout pipeline_layout;
    VkDescriptorPool desc_pool;
    std::vector<VkDescriptorSet> desc_set;

    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } uniform_data;

    struct {
        VkDescriptorImageInfo image_info;
    } texture_data;

    VkPipelineShaderStageCreateInfo shaderStages[2];

    VkPipelineCache pipelineCache;

    // vertex 相关
    struct {
        VkBuffer buf;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } vertex_buffer;

    VkVertexInputBindingDescription vi_binding;
    VkVertexInputAttributeDescription vi_attribs[2];

};


VkResult initVulkan();


void vulkan_init_instance_extension_name(struct vulkan_tutorial_info &info);

void vulkan_init_device_extension_name(struct vulkan_tutorial_info &info);

void vulkan_init_instance(struct vulkan_tutorial_info &info);

void vulkan_init_enumerate_device(struct vulkan_tutorial_info &info);

void vulkan_init_queue_family_and_index(struct vulkan_tutorial_info &info);

void vulkan_init_device(struct vulkan_tutorial_info &info);

void vulkan_init_swapchain_extension(struct vulkan_tutorial_info &info, ANativeWindow *window);

void vulkan_init_device_queue(struct vulkan_tutorial_info &info);

void vulkan_init_swapchain(struct vulkan_tutorial_info &info);

void vulkan_init_renderpass(struct vulkan_tutorial_info &info, bool clear = true,
                            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

void vulkan_init_framebuffer(struct vulkan_tutorial_info &info);

void vulkan_init_command_pool(struct vulkan_tutorial_info &info);

void vulkan_init_command_buffer(struct vulkan_tutorial_info &info);

void vulkan_execute_begin_command_buffer(struct vulkan_tutorial_info &info);

void vulkan_acquire_next_image(struct vulkan_tutorial_info &info, VkSemaphore &imageAcquiredSemaphore);

void vulkan_build_command(struct vulkan_tutorial_info &info);

void vulkan_submit_command(struct vulkan_tutorial_info &info, VkFence &drawFence, VkSemaphore imageAcquiredSemaphore);

void vulkan_present(struct vulkan_tutorial_info &info, VkFence &drawFence);

void vulkan_wait_second(int seconds);

void vulkan_destroy_device(struct vulkan_tutorial_info &info);

void vulkan_destroy_instance(struct vulkan_tutorial_info &info);

void vulkan_init_descriptor_set_layout_and_pipeline_layout(struct vulkan_tutorial_info &info,bool use_texture);

void vulkan_init_descriptor_pool(struct vulkan_tutorial_info &info,bool use_texture);

void vulkan_init_descriptor_set(struct vulkan_tutorial_info &info,bool use_texture);

void vulkan_init_shader(struct vulkan_tutorial_info &info,const char *ertShaderText, const char* fragShaderText);

bool vulkan_glsl_to_spv(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spir);

void vulkan_init_pipeline_cache(struct vulkan_tutorial_info &info);

void vulkan_init_pipeline(struct vulkan_tutorial_info &info,VkBool32 include_vi);

void vulkan_init_vertex_buffer(struct vulkan_tutorial_info &info,const void *vertexData, uint32_t dataSize, uint32_t dataStride,
                          bool use_texture);

void ErrorCheck(VkResult result);


#endif //VULKAN_TUTORIAL_VULKAN_UTIL_H
