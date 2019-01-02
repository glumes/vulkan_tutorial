//
// Created by glumes on 2018/12/11.
//

#include <vulkan_util.h>
#include "pipeline_and_descriptor_set.h"

void run(struct vulkan_tutorial_info &info) {

    ErrorCheck(initVulkan());

    vulkan_init_instance(info);
    vulkan_init_enumerate_device(info);
    vulkan_init_queue_family_and_index(info);
    vulkan_init_device(info);

    // VkDescriptorSetLayoutBinding 用于描述 descriptor set

    VkDescriptorSetLayoutBinding layoutBinding = {};

    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;


    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = nullptr;
    descriptor_layout.bindingCount = 1;
    descriptor_layout.pBindings = &layoutBinding;

    info.desc_layout.resize(NUM_DESCRIPTOR_SETS);

    VkResult res = vkCreateDescriptorSetLayout(info.device, &descriptor_layout, nullptr, info.desc_layout.data());

    ErrorCheck(res);

    /* Now use the descriptor layout to create a pipeline layout */

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pipelineLayoutCreateInfo.pSetLayouts = info.desc_layout.data();

    // DescriptorSetLayout 只是 DescriptorSet 的布局描述，还需要创建 descriptor set 实例对象来真正跟 buffer 或 image 关联

    res = vkCreatePipelineLayout(info.device, &pipelineLayoutCreateInfo, nullptr, &info.pipeline_layout);
    ErrorCheck(res);

    // create descriptor set

    VkDescriptorPoolSize type_count[1];
    type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_count[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = 1;
    descriptorPoolCreateInfo.pPoolSizes = type_count;

    res = vkCreateDescriptorPool(info.device, &descriptorPoolCreateInfo, nullptr, &info.desc_pool);

    // DescriptorSet 从 DescriptorPool 中分配
    // 多个线程同时更新描述符集  ？
    // DescriptorSet 的更新是 Vulkan 的优化项之一
    VkDescriptorSetAllocateInfo allocateInfo[1];
    allocateInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo[0].pNext = nullptr;
    allocateInfo[0].descriptorPool = info.desc_pool;
    allocateInfo[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
    allocateInfo[0].pSetLayouts = info.desc_layout.data();

    info.desc_set.resize(NUM_DESCRIPTOR_SETS);

    res = vkAllocateDescriptorSets(info.device, allocateInfo, info.desc_set.data());

    ErrorCheck(res);

    VkWriteDescriptorSet writes[1];
    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &info.uniform_data.buffer_info;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    vkUpdateDescriptorSets(info.device, 1, writes, 0, nullptr);

    // 管线状态用于创建管线对象
    // 一个性能优化的关键点，不用一次又一次的创建管线对象，希望创建一次并对其重用
    // 使用管线对象 、管线缓存对象 和 管线布局来控制状态

    // 管线对象 包含许多状态：着色器编译、资源绑定、渲染传递、
    // 管线布局 指定了 与管线一起使用的 描述符集

}


void destroy(struct vulkan_tutorial_info &info) {

    vkDestroyDescriptorPool(info.device, info.desc_pool, nullptr);

    vkDestroyBuffer(info.device, info.uniform_data.buf, nullptr);
    vkFreeMemory(info.device, info.uniform_data.mem, nullptr);


    for (int i = 0; i < NUM_DESCRIPTOR_SETS; ++i) {
        vkDestroyDescriptorSetLayout(info.device, info.desc_layout[1], nullptr);
    }

    vkDestroyPipelineLayout(info.device, info.pipeline_layout, nullptr);

    vulkan_destroy_device(info);
    vulkan_destroy_instance(info);
}
