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

    VkFence fence;
    VkFenceCreateInfo fence_info = {};
    // 默认是 unsignaled state
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(info.device, &fence_info, nullptr, &fence);

    VkSemaphore semaphore;
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(info.device, &semaphore_info, nullptr, &semaphore);

    VkCommandPool command_pool;
    VkCommandPoolCreateInfo poolCreateInfo = {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.queueFamilyIndex = info.graphics_queue_family_index;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(info.device, &poolCreateInfo, nullptr, &command_pool);

    VkCommandBuffer commandBuffer[2];
    VkCommandBufferAllocateInfo command_buffer_allocate_info{};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.commandBufferCount = 2;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(info.device, &command_buffer_allocate_info, commandBuffer);


    // 简单的提交过程
    VkCommandBufferBeginInfo beginInfo1 = {};
    beginInfo1.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo1.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer[0], &beginInfo1);

    // 省略中间的 vkCmdXXXX 系列方法

    vkEndCommandBuffer(commandBuffer[0]);

    VkSubmitInfo submitInfo1 = {};
    submitInfo1.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo1.commandBufferCount = 1;
    submitInfo1.pCommandBuffers = &commandBuffer[0];

    // pWaitSemaphores 和 pSignalSemaphores 都不设置，只是提交
    // 注意最后的参数 临时设置为 VK_NULL_HANDLE，也可以设置为  Fence 来同步
    vkQueueSubmit(info.queue, 1, &submitInfo1, VK_NULL_HANDLE);


    // wait fence to enter the signaled state on the host
//    VkResult res = vkWaitForFences(info.device, 1, &fence, VK_TRUE, UINT64_MAX);
    VkResult res;
    LOGD("start wait");
    do {
        res = vkWaitForFences(info.device, 1, &fence, VK_TRUE, UINT64_MAX);
        LOGD("wait");
    } while (res == VK_TIMEOUT);

    LOGD("fence enter the signaled state");

//
//
//    {
//        VkCommandBufferBeginInfo beginInfo = {};
//        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//        vkBeginCommandBuffer(commandBuffer[0], &beginInfo);
//
//        vkCmdPipelineBarrier(commandBuffer[0], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
//                             0,
//                             0, nullptr,
//                             0, nullptr,
//                             0, nullptr);
//
//        VkViewport viewport = {};
//
//        viewport.maxDepth = 1.0f;
//        viewport.minDepth = 0.0f;
//        viewport.width = 512;
//        viewport.height = 512;
//        viewport.x = 0;
//        viewport.y = 0;
//
//        vkCmdSetViewport(commandBuffer[0], 0, 1, &viewport);
//
//        vkEndCommandBuffer(commandBuffer[0]);
//
//    }
//
//    {
//        VkCommandBufferBeginInfo beginInfo = {};
//        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        vkBeginCommandBuffer(commandBuffer[1], &beginInfo);
//
//
//        VkViewport viewport = {};
//
//        viewport.maxDepth = 1.0f;
//        viewport.minDepth = 0.0f;
//        viewport.width = 512;
//        viewport.height = 512;
//        viewport.x = 0;
//        viewport.y = 0;
//
//        vkCmdSetViewport(commandBuffer[1], 0, 1, &viewport);
//
//        vkEndCommandBuffer(commandBuffer[1]);
//
//    }
//
//    {
//        VkSubmitInfo submitInfo = {};
//        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//        submitInfo.commandBufferCount = 1;
//        submitInfo.pCommandBuffers = &commandBuffer[0];
//
//        // signal semaphore
//        submitInfo.signalSemaphoreCount = 1;
//        submitInfo.pSignalSemaphores = &semaphore;
//
//
//        vkQueueSubmit(info.queue, 1, &submitInfo, VK_NULL_HANDLE);
//    }
//
//    {
//        VkPipelineStageFlags flags[] = {
//                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
//        };
//
//        VkSubmitInfo submitInfo = {};
//        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//        submitInfo.commandBufferCount = 1;
//        submitInfo.pCommandBuffers = &commandBuffer[1];
//
//        // wait semaphore
//        submitInfo.waitSemaphoreCount = 1;
//        submitInfo.pWaitSemaphores = &semaphore;
//        submitInfo.pWaitDstStageMask = flags;
//
//        vkQueueSubmit(info.queue, 1, &submitInfo, VK_NULL_HANDLE);
//    }

    vkQueueWaitIdle(info.queue);

    vkDestroyCommandPool(info.device, command_pool, nullptr);
    vkDestroyFence(info.device, fence, nullptr);
    vkDestroySemaphore(info.device, semaphore, nullptr);

    LOGD("finish run");
}

void destroy(struct vulkan_tutorial_info &info) {
    vulkan_destroy_device(info);
    vulkan_destroy_instance(info);
}
