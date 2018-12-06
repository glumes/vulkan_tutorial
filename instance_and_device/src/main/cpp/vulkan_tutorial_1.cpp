#include <jni.h>
#include <string>
#include <vector>
// vulkan_wrapper.h 要在 vulkan 前面才行
#include <vulkan_wrapper.h>
#include <vulkan/vulkan.h>
#include <logutil.h>

#include <iostream>
#include <cstdlib>
#include <vulkan_util.h>

static VkInstance instance = nullptr;
static VkDevice device = nullptr;

#define ENGINE_NAME "EngineName"
#define APPLICATION_NAME "ApplicationName"

using namespace std;

extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_tutorial_11_VulkanTutorial_initInstance(JNIEnv *env, jclass type) {

    LOGI("init instance");

    ErrorCheck(initVulkan());

    VkApplicationInfo app_info = {};

    app_info.apiVersion = VK_API_VERSION_1_0;
    app_info.applicationVersion = 1;
    app_info.engineVersion = 1;
    app_info.pNext = nullptr;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pEngineName = APPLICATION_NAME;
    app_info.pApplicationName = APPLICATION_NAME;


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

    VkResult result = vkCreateInstance(&instance_info, nullptr, &instance);

    ErrorCheck(result);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_tutorial_11_VulkanTutorial_initDevice(JNIEnv *env, jclass type) {

    LOGI("init device");

    if (instance == nullptr) {
        LOGE("init instance first");
        return;
    }

    LOGI("enumerate gpu device");

    uint32_t gpu_size = 0;
    VkResult res = vkEnumeratePhysicalDevices(instance, &gpu_size, nullptr);
    ErrorCheck(res);
    assert(gpu_size != 0);

    LOGI("physical device size is %d", gpu_size);

    vector<VkPhysicalDevice> gpus;
    gpus.resize(gpu_size);

    // vector.data() 方法转换成指针类型
    res = vkEnumeratePhysicalDevices(instance, &gpu_size, gpus.data());
    ErrorCheck(res);

    assert(gpu_size >= 1);


    LOGI("get physical device properties");

    VkDeviceQueueCreateInfo queue_info = {};

    uint32_t queue_family_count = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queue_family_count, nullptr);
    assert(queue_family_count != 0);


    vector<VkQueueFamilyProperties> queue_family_props;
    queue_family_props.resize(queue_family_count);

    vkGetPhysicalDeviceQueueFamilyProperties(gpus[0], &queue_family_count, queue_family_props.data());
    assert(queue_family_count != 0);


    bool found = false;
    for (unsigned int i = 0; i < queue_family_count; ++i) {
        if (queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_info.queueFamilyIndex = i;
            found = true;
            break;
        }
    }

    assert(found);

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

    res = vkCreateDevice(gpus[0], &device_info, nullptr, &device);

    ErrorCheck(res);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_tutorial_11_VulkanTutorial_destroyInstance(JNIEnv *env, jclass type) {

    LOGI("destroyInstance");

    if (device != nullptr) {
        vkDestroyDevice(device, nullptr);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_tutorial_11_VulkanTutorial_destroyDevice(JNIEnv *env, jclass type) {

    LOGI("destroyDevice");

    if (instance != nullptr) {
        vkDestroyInstance(instance, nullptr);
    }
}
