//
// Created by glumes on 2018/12/6.
//

#include "swapchain_and_surface.h"


// 创建 swapchain ，然后 swapchain 生成 image ,在生成 image 对应的 imageview
// 最后再销毁 swapchain

void run(struct vulkan_tutorial_info &info, ANativeWindow *window, int width, int height) {

    ErrorCheck(initVulkan());



    GET_INSTANCE_PROC_ADDR(info.instance, CreateAndroidSurfaceKHR);

    // Instance 的拓展
    vulkan_init_instance_extension_name(info);

    // Device 的拓展
    vulkan_init_device_extension_name(info);

    vulkan_init_instance(info);

    vulkan_init_enumerate_device(info);

    vulkan_init_queue_family_and_index(info);




    // 在 init device 之前 init swapchain
    vulkan_init_device(info);

    VkAndroidSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.window = window;

    VkResult res = info.fpCreateAndroidSurfaceKHR(info.instance, &createInfo, nullptr, &info.surface);


    ErrorCheck(res);

    // 先将所有的 index 都初始化到一个最大值，好用于后续的判断过程
    info.graphics_queue_family_index = UINT32_MAX;
    info.present_queue_family_index = UINT32_MAX;

    assert(res);

    VkBool32 *supportPresent = static_cast<VkBool32 *>(malloc(info.queue_family_size * sizeof(VkBool32)));

    for (uint32_t i = 0; i < info.queue_family_size; i++) {
        // physical device 的 queue 是否支持 surface
        vkGetPhysicalDeviceSurfaceSupportKHR(info.gpu_physical_devices[0], i, info.surface, &supportPresent[i]);
    }

    // 得到 surface 之后，选择 queue family queue index
    // 然后再查询设备对该 surface 支持的能力
    // 获得兼容的队列


    for (uint32_t i = 0; i < info.queue_family_size; ++i) {

        if ((info.queue_family_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (info.graphics_queue_family_index == UINT32_MAX) {
                info.graphics_queue_family_index = i;
            }

            // 找到了即支持 graphics 也支持 present 的 queue index
            if (supportPresent[i] == VK_TRUE) {
                info.graphics_queue_family_index = i;
                info.present_queue_family_index = i;
                break;
            }
        }
    }

    // 没有找到一个 queue 两者都支持，那么找到一个支持 present 的 queue family index
    if (info.present_queue_family_index == UINT32_MAX) {
        for (size_t i = 0; i < info.queue_family_size; ++i) {
            if (supportPresent[i] == VK_TRUE) {
                info.present_queue_family_index = i;
                break;
            }
        }
    }
    free(supportPresent);

    // 如果都没找到，就报错了
    if (info.graphics_queue_family_index == UINT32_MAX || info.present_queue_family_index == UINT32_MAX) {
        LOGE("could not find a queue for graphics and present");
    }


    // 查询表面格式
    // 检索物理设备支持的所有公开的表面格式

    uint32_t formatCount;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpu_physical_devices[0], info.surface, &formatCount, nullptr);

    ErrorCheck(res);

    VkSurfaceFormatKHR *surfaceFormats = static_cast<VkSurfaceFormatKHR *>(malloc(
            formatCount * sizeof(VkSurfaceFormatKHR)));

    res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpu_physical_devices[0], info.surface, &formatCount,
                                               surfaceFormats);


    // 查询交换链图形格式
    // 交换链需要一种表面支持的色彩空间格式 也就是 surface 支持的色彩空间格式。
    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.

    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        info.format = VK_FORMAT_UNDEFINED;
    } else {
        info.format = surfaceFormats[0].format;
    }

    free(surfaceFormats);

    // 创建交换链

    // 物理设备表面的 能力

    // 这里是查询物理设备相关的能力

    // 物理设备提供的图像表面特性

    // 也就是 surface 的表面特性

    VkSurfaceCapabilitiesKHR surfaceCapabilitiesKHR;


    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpu_physical_devices[0], info.surface,
                                                    &surfaceCapabilitiesKHR);

    ErrorCheck(res);


    uint32_t presentModeCount;
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpu_physical_devices[0], info.surface, &presentModeCount,
                                                    NULL);
    assert(res == VK_SUCCESS);
    VkPresentModeKHR *presentModes = (VkPresentModeKHR *) malloc(presentModeCount * sizeof(VkPresentModeKHR));

    res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpu_physical_devices[0], info.surface, &presentModeCount,
                                                    presentModes);
    assert(res == VK_SUCCESS);


    VkExtent2D swapchainExtent;

    if (surfaceCapabilitiesKHR.currentExtent.width == 0xFFFFFFFF) {
        swapchainExtent.width = info.width;
        swapchainExtent.height = info.height;

        if (swapchainExtent.width < surfaceCapabilitiesKHR.minImageExtent.width) {
            swapchainExtent.width = surfaceCapabilitiesKHR.minImageExtent.width;
        } else if (swapchainExtent.width > surfaceCapabilitiesKHR.maxImageExtent.width) {
            swapchainExtent.width = surfaceCapabilitiesKHR.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfaceCapabilitiesKHR.minImageExtent.height) {
            swapchainExtent.height = surfaceCapabilitiesKHR.minImageExtent.height;
        } else if (swapchainExtent.height > surfaceCapabilitiesKHR.maxImageExtent.height) {
            swapchainExtent.height = surfaceCapabilitiesKHR.maxImageExtent.height;
        }
    } else {
        swapchainExtent = surfaceCapabilitiesKHR.currentExtent;
    }

    // 物理设备表面的呈现 模式
    // 物理设备的展示模式

    // 交换链包含的彩色图像 由 展示引擎 使用展示方案进行管理。

    // 这些方案用来确定传入的展示请求将会如何在内部进行处理以及队列化。

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t desiredNumberOfSwapChainImages = surfaceCapabilitiesKHR.minImageCount;

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfaceCapabilitiesKHR.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfaceCapabilitiesKHR.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
        if (surfaceCapabilitiesKHR.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }


    // 创建一个 swapchain 需要 4 个条件
    // VkExtent2D
    // VkPresentModeKHR
    // VkSurfaceTransformFlagBitsKHR
    // VkCompositeAlphaFlagBitsKHR


    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.pNext = NULL;
    swapchain_ci.surface = info.surface;
    swapchain_ci.minImageCount = desiredNumberOfSwapChainImages;
    swapchain_ci.imageFormat = info.format;
    swapchain_ci.imageExtent.width = swapchainExtent.width;
    swapchain_ci.imageExtent.height = swapchainExtent.height;
    swapchain_ci.preTransform = preTransform;
    swapchain_ci.compositeAlpha = compositeAlpha;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.presentMode = swapchainPresentMode;
    swapchain_ci.oldSwapchain = VK_NULL_HANDLE;
    swapchain_ci.clipped = true;
    swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = NULL;
    uint32_t queueFamilyIndices[2] = {(uint32_t) info.graphics_queue_family_index,
                                      (uint32_t) info.present_queue_family_index};
    if (info.graphics_queue_family_index != info.present_queue_family_index) {
        // If the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between
        // the queues, or we have to create the swapchain with imageSharingMode
        // as VK_SHARING_MODE_CONCURRENT
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_ci.queueFamilyIndexCount = 2;
        swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;
    }

    res = vkCreateSwapchainKHR(info.device, &swapchain_ci, NULL, &info.swap_chain);
    assert(res == VK_SUCCESS);


    // 得到 swapchain image

    // 成功创建交换链后，VkImage 对象 图像表面就会在幕后创建。
    // 在获取图像表面之前，我们分配了足够的内存空间来容纳图像缓冲区。

    // 交换链图形和物理表面的数量又 如下函数返回
    res = vkGetSwapchainImagesKHR(info.device, info.swap_chain, &info.swapchainImageCount, NULL);
    assert(res == VK_SUCCESS);

    VkImage *swapchainImages = (VkImage *) malloc(info.swapchainImageCount * sizeof(VkImage));
    assert(swapchainImages);

    res = vkGetSwapchainImagesKHR(info.device, info.swap_chain, &info.swapchainImageCount, swapchainImages);
    assert(res == VK_SUCCESS);


    info.buffers.resize(info.swapchainImageCount);
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        info.buffers[i].image = swapchainImages[i];
    }
    free(swapchainImages);


    // 创建彩色图像视图
    // 应用程序不会直接以图像对象的形式使用图像
    // 而是通过图像视图的方式


    // 对每个交换链图像，通过遍历来可用的图像对象列表来创建相应的图像视图

    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.flags = 0;
        color_image_view.image = info.buffers[i].image;
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.format = info.format;
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;


        res = vkCreateImageView(info.device, &color_image_view, NULL, &info.buffers[i].view);
        assert(res == VK_SUCCESS);
    }




//    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
//        vkDestroyImageView(info.device,info.buffers[i].view, nullptr);
//    }
//    vkDestroySwapchainKHR(info.device,info.swap_chain, nullptr);

//    destroy(info);
}

