//
// Created by glumes on 2018/12/6.
//

#ifndef VULKAN_TUTORIAL_VULKAN_UTIL_H
#define VULKAN_TUTORIAL_VULKAN_UTIL_H


#include <vulkan_wrapper.h>
#include <vulkan/vulkan.h>
#include <logutil.h>

VkResult initVulkan();

void ErrorCheck(VkResult result);



#endif //VULKAN_TUTORIAL_VULKAN_UTIL_H
