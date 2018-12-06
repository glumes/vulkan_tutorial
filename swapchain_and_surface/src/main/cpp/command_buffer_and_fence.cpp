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
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
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



    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(commandBuffer[0], &beginInfo);
        vkCmdPipelineBarrier(commandBuffer[0], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                             0,
                             0, nullptr,
                             0, nullptr,
                             0, nullptr);

        VkViewport viewport = {};

        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;
        viewport.width = 512;
        viewport.height = 512;
        viewport.x = 0;
        viewport.y = 0;

        vkCmdSetViewport(commandBuffer[0], 0, 1, &viewport);

        vkEndCommandBuffer(commandBuffer[0]);

    }

    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(commandBuffer[0], &beginInfo);

        vkCmdPipelineBarrier(commandBuffer[0], VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                             0,
                             0, nullptr,
                             0, nullptr,
                             0, nullptr);

        VkViewport viewport = {};

        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;
        viewport.width = 512;
        viewport.height = 512;
        viewport.x = 0;
        viewport.y = 0;

        vkCmdSetViewport(commandBuffer[0], 0, 1, &viewport);

        vkEndCommandBuffer(commandBuffer[0]);

    }

    {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer[0];

        // signal semaphore
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &semaphore;


        vkQueueSubmit(info.queue, 1, &submitInfo, VK_NULL_HANDLE);
    }

    {
        VkPipelineStageFlags flags[] = {
                VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
        };

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer[0];

        // wait semaphore
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &semaphore;
        submitInfo.pWaitDstStageMask = flags;

        vkQueueSubmit(info.queue, 1, &submitInfo, VK_NULL_HANDLE);
    }

    vkQueueWaitIdle(info.queue);

    vkDestroyCommandPool(info.device, command_pool, nullptr);
    vkDestroyFence(info.device, fence, nullptr);
    vkDestroySemaphore(info.device, semaphore, nullptr);
}

void destroy(struct vulkan_tutorial_info &info) {
    vulkan_destroy_device(info);
    vulkan_destroy_instance(info);
}
