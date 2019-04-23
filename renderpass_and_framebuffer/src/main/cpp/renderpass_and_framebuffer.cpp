//
// Created by glumes on 2019/3/25.
//

#include "renderpass_and_framebuffer.h"


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

    VkSemaphore imageAcquiredSemaphore;

    VkFence drawFence;

    vulkan_acquire_next_image(info, imageAcquiredSemaphore);

    vulkan_build_command(info);

    vulkan_submit_command(info, drawFence, imageAcquiredSemaphore);

    vulkan_present(info, drawFence);

}

