/*
#include <jni.h>
#include <string>
#include <vector>
// vulkan_wrapper.h 要在 vulkan 前面才行
#include <vulkan_wrapper.h>
#include <vulkan/vulkan.h>
#include <logutil.h>

#include <iostream>
#include <cstdlib>

#define INFO_NAME "vulkan_tutorial"


static VkInstance inst = nullptr;
extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_tutorial_11_VulkanTutorial_initInstance(JNIEnv *env, jclass type) {

    if (!InitVulkan()) {
        LOGE("failed to init vulkan apis");
        return;
    } else {
        LOGE("init success");
    }*/
/**//*


    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;

    app_info.pApplicationName = INFO_NAME;
    app_info.applicationVersion = 1;

    app_info.pEngineName = INFO_NAME;
    app_info.engineVersion = 1;

    app_info.apiVersion = VK_API_VERSION_1_0;


    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = nullptr;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;

    inst_info.enabledExtensionCount = 0;
    inst_info.ppEnabledExtensionNames = nullptr;

    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = nullptr;

//    VkInstance inst;

    VkResult res;

    res = vkCreateInstance(&inst_info, nullptr, &inst);

    if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
//        std::cout << "C";
        LOGE("error");
    } else if (res) {
        std::cout << "unknown error";
        LOGE("unknown error");
    } else {
        std::cout << "success";
        LOGE("success");
    }

//    vkDestroyInstance(inst, nullptr);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_tutorial_11_VulkanTutorial_initDevice(JNIEnv *env, jclass type) {

    uint32_t gpu_count = 1;

    // 枚举 physical device

    VkResult res = vkEnumeratePhysicalDevices(inst, &gpu_count, nullptr);
    LOGD("gpu_count is %d", gpu_count);
    // 手机上的 gpu 就只有一个了

    std::vector<VkPhysicalDevice> gpus;

    gpus.reserve(gpu_count);

    res = vkEnumeratePhysicalDevices(inst, &gpu_count, gpus.data());

    if (res != VK_SUCCESS) {
        LOGD("get physical device failed");
        return;
    }

    // 得到物理设备之后，查询相关信息

    // 首先查询 PhysicalDeviceQueueFamilyProperties


    // 除此之前，还可以得到

//    vkGetPhysicalDeviceDisplayPlanePropertiesKHR
//    vkGetPhysicalDeviceProperties
//    vkGetPhysicalDeviceMemoryProperties
//    vkGetPhysicalDeviceFormatProperties
//    vkGetPhysicalDeviceDisplayPropertiesKHR


    VkDeviceQueueCreateInfo queue_info = {};

    uint32_t queue_family_count;

    vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queue_family_count, nullptr);

    // queue family 数量也是 1
    std::vector<VkQueueFamilyProperties> queue_family_props;

    LOGD("queue family count is %d", queue_family_count);

    queue_family_props.resize(queue_family_count);

    vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queue_family_count, queue_family_props.data());

    // 查询 device 的 queue

    // 打印 queue family 结构体的信息

    for (int i = 0; i < queue_family_count; ++i) {
        if (queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_info.queueFamilyIndex = i;
        }
    }

    // 接下来才是创建 device
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
    device_info.ppEnabledExtensionNames = nullptr;
    device_info.enabledLayerCount = 0;
    // ppEnableExtensionNames 可以被查询到
    device_info.ppEnabledLayerNames = nullptr;
    // pEnableFeatures 也可以被查询到
    device_info.pEnabledFeatures = nullptr;

    VkDevice device;
    res = vkCreateDevice(gpus[0], &device_info, nullptr, &device);

    if (res != VK_SUCCESS) {
        LOGE("create device failed");
    } else {
        LOGE("create device success");
    }


    */
/**
     * 接下来 create command pool
     * 有一个 VkCommandPoolCreateInfo 来表示要创建的信息
     * 然后调用 vkCreateCommandPool 创建
     *
     * 从 command pool 中创建 command buffer
     * 用 VkCommandBufferAllocateInfo 表示要创建的信息
     * 然后调用 vkAllocateCommandBuffers 进行创建
     *
     * 最后通过：vkFreeCommandBuffers 销毁 command buffer
     * 再通过：vkDestroyCommandPool 销毁 command pool
     *//*



    */
/**
     * 创建 swapchain
     *
     * 用 VkAndroidSurfaceCreateInfoKHR 来表示要创建的 surface
     * 通过 PFN_vkCreateAndroidSurfaceKHR 函数来创建 Surface
     *
     * 展示层（presentation layer）使用 vulkan 的窗口系统集成 Window System Integration (WSI) API 进行初始化
     * WSI 将用于渲染期间在显示表面（display surface）上绘制图形图形。
     *
     * 应用程序创建资源并使用描述符（descriptors）将它们绑定到着色器阶段（shader stage）
     * 描述符集布局（descriptor set layout）用与把创建的资源绑定到创建的底层管线对象（pipeline object）(图形或者计算类型 graphics or compute type)
     * 最后，记录命令缓冲区并将其提交到队列进行处理
     *
     * Image 对应的就是图像
     * Vulkan 中的上下文或者设备的创建根本不需要涉及窗口系统，它会通过窗口系统集成进行管理
     *
     * WSI 提供了交换链机制，这允许以这样的方式使用多个图像，即当窗口图像显示一个图像时，应用程序可以准备下一个图像
     *
     * 使用时序：
     * 1.创建一个本地窗口
     * 2.创建一个 WSI 表面并附加到窗口
     * 3.创建交换链，用于呈现到表面
     * 4.从创建的交换链请求绘图图像
     *//*

}



extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_tutorial_11_VulkanTutorial_destroyInstance(JNIEnv *env, jclass type) {

    // TODO

}*/
