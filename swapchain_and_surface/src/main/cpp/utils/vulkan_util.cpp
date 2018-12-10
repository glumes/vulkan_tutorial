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


void vulkan_init_layer_and_extension_properties(vulkan_tutorial_info &info) {


}


void vulkan_init_instance(struct vulkan_tutorial_info &info) {

    VkApplicationInfo app_info = {};

    app_info.apiVersion = VK_API_VERSION_1_0;
    app_info.applicationVersion = 1;
    app_info.engineVersion = 1;
    app_info.pNext = nullptr;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pEngineName = APP_SHORT_NAME;
    app_info.pApplicationName = APP_SHORT_NAME;


    VkInstanceCreateInfo instance_info = {};

    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pNext = nullptr;
    instance_info.pApplicationInfo = &app_info;
    instance_info.flags = 0;
    // Extension and Layer
    instance_info.enabledExtensionCount = 0;
    instance_info.ppEnabledExtensionNames = nullptr;
    instance_info.ppEnabledLayerNames = nullptr;
    instance_info.enabledLayerCount = 0;

    VkResult res = vkCreateInstance(&instance_info, nullptr, &info.instance);

    ErrorCheck(res);
}


void vulkan_init_enumerate_device(struct vulkan_tutorial_info &info) {
    if (info.instance == nullptr) {
        return;
    }

    VkResult res = vkEnumeratePhysicalDevices(info.instance, &info.gpu_size, nullptr);
    ErrorCheck(res);
    assert(info.gpu_size != 0);

    info.gpu_physical_devices.resize(info.gpu_size);

    res = vkEnumeratePhysicalDevices(info.instance, &info.gpu_size, info.gpu_physical_devices.data());
    ErrorCheck(res);
    assert(info.gpu_size != 0);
}


void vulkan_init_queue_family_and_index(struct vulkan_tutorial_info &info) {
    if (info.instance == nullptr) {
        return;
    }


    vkGetPhysicalDeviceQueueFamilyProperties(info.gpu_physical_devices[0], &info.queue_family_size,
                                             nullptr);
    assert(info.queue_family_size != 0);
    info.queue_family_props.resize(info.queue_family_size);

    vkGetPhysicalDeviceQueueFamilyProperties(info.gpu_physical_devices[0], &info.queue_family_size,
                                             info.queue_family_props.data());
    assert(info.queue_family_size != 0);

    bool found = false;
    for (unsigned int i = 0; i < info.queue_family_size; i++) {
        if (info.queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            info.graphics_queue_family_index = i;
            found = true;
            break;
        }
    }
    assert(found);
}


void vulkan_init_device(struct vulkan_tutorial_info &info) {

    VkDeviceQueueCreateInfo queue_info = {};

    float queue_priorities[1] = {0.0};

    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = nullptr;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;


    VkDeviceCreateInfo device_info = {};

    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = nullptr;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;

    device_info.enabledExtensionCount = 0;
    device_info.ppEnabledExtensionNames = NULL;
    device_info.enabledLayerCount = 0;
    device_info.ppEnabledLayerNames = NULL;
    device_info.pEnabledFeatures = NULL;

    VkResult res = vkCreateDevice(info.gpu_physical_devices[0], &device_info, nullptr, &info.device);

    ErrorCheck(res);

    // todo
    vkGetDeviceQueue(info.device, info.graphics_queue_family_index, 0, &info.queue);
}


void vulkan_destroy_device(struct vulkan_tutorial_info &info) {
    if (info.device != nullptr) {
        vkDestroyDevice(info.device, nullptr);
    }
}


void vulkan_destroy_instance(struct vulkan_tutorial_info &info) {
    if (info.instance != nullptr) {
        vkDestroyInstance(info.instance, nullptr);
    }
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


