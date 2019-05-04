//
// Created by glumes on 2018/12/11.
//

#include <vulkan_util.h>
#include "pipeline_and_descriptor_set.h"

void run(struct vulkan_tutorial_info &info, ANativeWindow *window, int width, int height) {

    ErrorCheck(initVulkan());

//    GET_INSTANCE_PROC_ADDR(info.instance, CreateAndroidSurfaceKHR);

    // Instance 的拓展
    vulkan_init_instance_extension_name(info);

    // Device 的拓展
    vulkan_init_device_extension_name(info);

    vulkan_init_instance(info);

    vulkan_init_enumerate_device(info);

    vulkan_init_queue_family_and_index(info);


    info.width = width;
    info.height = height;

    // 在 init device 之前 init swapchain
    vulkan_init_swapchain_extension(info, window);


    vulkan_init_device(info);

    vulkan_init_command_pool(info);

    vulkan_init_command_buffer(info);

    vulkan_execute_begin_command_buffer(info);

    vulkan_init_device_queue(info);

    vulkan_init_swapchain(info);

    vulkan_init_renderpass(info);

    vulkan_init_framebuffer(info);


    vulkan_init_descriptor_set_layout_and_pipeline_layout(info, false);

    vulkan_init_descriptor_pool(info, false);

    vulkan_init_descriptor_set(info, false);

    VkSemaphore imageAcquiredSemaphore;

    VkFence drawFence;

    vulkan_acquire_next_image(info, imageAcquiredSemaphore);

    vulkan_build_command(info);

    vulkan_submit_command(info, drawFence, imageAcquiredSemaphore);

    vulkan_present(info, drawFence);



    // 管线状态用于创建管线对象
    // 一个性能优化的关键点，不用一次又一次的创建管线对象，希望创建一次并对其重用
    // 使用管线对象 、管线缓存对象 和 管线布局来控制状态

    // 管线对象 包含许多状态：着色器编译、资源绑定、渲染传递、
    // 管线布局 指定了 与管线一起使用的 描述符集

}


void destroy(struct vulkan_tutorial_info &info) {

//    vkDestroyDescriptorPool(info.device, info.desc_pool, nullptr);
//
//    vkDestroyBuffer(info.device, info.uniform_data.buf, nullptr);
//    vkFreeMemory(info.device, info.uniform_data.mem, nullptr);
//
//
//    for (int i = 0; i < NUM_DESCRIPTOR_SETS; ++i) {
//        vkDestroyDescriptorSetLayout(info.device, info.desc_layout[1], nullptr);
//    }
//
//    vkDestroyPipelineLayout(info.device, info.pipeline_layout, nullptr);
//
//    vulkan_destroy_device(info);
//    vulkan_destroy_instance(info);
}
