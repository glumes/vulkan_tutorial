//
// Created by glumes on 2018/12/6.
//

#include "command_buffer_and_fence.h"

void run(struct vulkan_tutorial_info &info) {
    ErrorCheck(initVulkan());

    vulkan_init_instance(info);
    vulkan_init_enumerate_device(info);
    vulkan_init_queue_family_and_index(info);
    vulkan_init_device(info);
}

void destroy(struct vulkan_tutorial_info &info) {
    vulkan_destroy_device(info);
    vulkan_destroy_instance(info);
}
