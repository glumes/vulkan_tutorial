//
// Created by glumes on 2018/12/6.
//

#include "vulkan_util.h"

VkResult initVulkan() {
    if (!InitVulkan()) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}



void ErrorCheck(VkResult result) {
    if (result < 0) {
        switch (result) {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                LOGE("VK_ERROR_OUT_OF_HOST_MEMORY");
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                LOGE("VK_ERROR_OUT_OF_DEVICE_MEMORY");
                break;
            case VK_ERROR_INITIALIZATION_FAILED:
                LOGE("VK_ERROR_INITIALIZATION_FAILED");
                break;
            case VK_ERROR_DEVICE_LOST:
                LOGE("VK_ERROR_DEVICE_LOST");
                break;
            case VK_ERROR_MEMORY_MAP_FAILED:
                LOGE("VK_ERROR_MEMORY_MAP_FAILED");
                break;
            case VK_ERROR_LAYER_NOT_PRESENT:
                LOGE("VK_ERROR_LAYER_NOT_PRESENT");
                break;
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                LOGE("VK_ERROR_EXTENSION_NOT_PRESENT");
                break;
            case VK_ERROR_FEATURE_NOT_PRESENT:
                LOGE("VK_ERROR_FEATURE_NOT_PRESENT");
                break;
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                LOGE("VK_ERROR_INCOMPATIBLE_DRIVER");
                break;
            case VK_ERROR_TOO_MANY_OBJECTS:
                LOGE("VK_ERROR_TOO_MANY_OBJECTS");
                break;
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                LOGE("VK_ERROR_FORMAT_NOT_SUPPORTED");
                break;
            case VK_ERROR_SURFACE_LOST_KHR:
                LOGE("VK_ERROR_SURFACE_LOST_KHR");
                break;
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                LOGE("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
                break;
            case VK_SUBOPTIMAL_KHR:
                LOGE("VK_SUBOPTIMAL_KHR");
                break;
            case VK_ERROR_OUT_OF_DATE_KHR:
                LOGE("VK_ERROR_OUT_OF_DATE_KHR");
                break;
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                LOGE("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
                break;
            case VK_ERROR_VALIDATION_FAILED_EXT:
                LOGE("VK_ERROR_VALIDATION_FAILED_EXT");
                break;
            default:
                break;
        }
    }
}

