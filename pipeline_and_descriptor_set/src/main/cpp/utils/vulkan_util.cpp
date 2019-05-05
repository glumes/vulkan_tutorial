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
    instance_info.enabledExtensionCount = info.instance_extension_names.size();
    instance_info.ppEnabledExtensionNames = info.instance_extension_names.data();
    // 这里没有用到 Layer 就还是设为 null 就好了
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

    // 在这一步反而不需要找到用于渲染的 queue flag，可以在后续创建 swapchain 的时候一起查找了
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
    queue_info.queueFamilyIndex = info.graphics_queue_family_index;

    VkDeviceCreateInfo device_info = {};

    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = nullptr;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;

    device_info.enabledExtensionCount = info.device_extension_names.size();
    device_info.ppEnabledExtensionNames = info.device_extension_names.data();
    // 这里没有用到 Layer 就还是设为 null 就好了
    device_info.enabledLayerCount = 0;
    device_info.ppEnabledLayerNames = NULL;
    device_info.pEnabledFeatures = NULL;

    VkResult res = vkCreateDevice(info.gpu_physical_devices[0], &device_info, nullptr, &info.device);

    ErrorCheck(res);

    // todo
//    vkGetDeviceQueue(info.device, info.graphics_queue_family_index, 0, &info.queue);
}

void vulkan_init_device_queue(struct vulkan_tutorial_info &info) {
    vkGetDeviceQueue(info.device, info.graphics_queue_family_index, 0, &info.graphics_queue);
    if (info.graphics_queue_family_index == info.present_queue_family_index) {
        info.present_queue = info.graphics_queue;
    } else {
        vkGetDeviceQueue(info.device, info.present_queue_family_index, 0, &info.present_queue);
    }
}


void vulkan_init_swapchain_extension(struct vulkan_tutorial_info &info, ANativeWindow *window) {


    GET_INSTANCE_PROC_ADDR(info.instance, CreateAndroidSurfaceKHR);


    VkAndroidSurfaceCreateInfoKHR createInfo;

    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.window = window;

    VkResult res = info.fpCreateAndroidSurfaceKHR(info.instance, &createInfo, nullptr, &info.surface);


    LOGE("debug");

    assert(res == VK_SUCCESS);

    // 先将所有的 index 都初始化到一个最大值，好用于后续的判断过程
    info.graphics_queue_family_index = UINT32_MAX;
    info.present_queue_family_index = UINT32_MAX;


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


    LOGE("present");

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

}

void vulkan_init_swapchain(struct vulkan_tutorial_info &info) {

    VkResult res;

    VkSurfaceCapabilitiesKHR surfaceCapabilitiesKHR;

    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpu_physical_devices[0], info.surface,
                                                    &surfaceCapabilitiesKHR);

    assert(res == VK_SUCCESS);

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

//
    info.buffers.resize(info.swapchainImageCount);
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        info.buffers[i].image = swapchainImages[i];
    }


    // init swap chain image

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

    free(swapchainImages);

    info.current_buffer = 0;

    if (presentModes != nullptr) {
        free(presentModes);
    }

}


void vulkan_init_renderpass(struct vulkan_tutorial_info &info, bool clear, VkImageLayout finalLayout) {

    VkResult res;

    VkAttachmentDescription attachments[1];

    attachments[0].format = info.format;
    attachments[0].samples = NUM_SAMPLES;
    attachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = finalLayout;
    attachments[0].flags = 0;

    VkAttachmentReference color_reference = {};
    color_reference.attachment = 0;
    color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};

    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags = 0;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_reference;

    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = nullptr;

    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.pNext = nullptr;
    rp_info.attachmentCount = 1;
    rp_info.pAttachments = attachments;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    rp_info.dependencyCount = 0;
    rp_info.pDependencies = nullptr;

    res = vkCreateRenderPass(info.device, &rp_info, nullptr, &info.render_pass);

    ErrorCheck(res);
}

void vulkan_init_framebuffer(struct vulkan_tutorial_info &info) {
    VkResult res;

    VkImageView attachments[2];
    attachments[1] = info.depth.view;

    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;
    fb_info.renderPass = info.render_pass;
    fb_info.attachmentCount = 1;
    fb_info.pAttachments = attachments;
    fb_info.width = info.width;
    fb_info.height = info.height;

    fb_info.layers = 1;

    uint32_t i;

    info.framebuffers = static_cast<VkFramebuffer *>(malloc(info.swapchainImageCount * sizeof(VkFramebuffer)));

    for (int i = 0; i < info.swapchainImageCount; ++i) {
        attachments[0] = info.buffers[i].view;
        res = vkCreateFramebuffer(info.device, &fb_info, nullptr, &info.framebuffers[i]);
        assert(res == VK_SUCCESS);
    }
}


void vulkan_init_command_pool(struct vulkan_tutorial_info &info) {

    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = nullptr;
    cmd_pool_info.queueFamilyIndex = info.graphics_queue_family_index;
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult res = vkCreateCommandPool(info.device, &cmd_pool_info, nullptr, &info.cmd_pool);
    assert(res == VK_SUCCESS);
}

void vulkan_init_command_buffer(struct vulkan_tutorial_info &info) {
    VkResult res;
    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext = nullptr;
    cmd.commandPool = info.cmd_pool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(info.device, &cmd, &info.cmd);
    assert(res == VK_SUCCESS);

}

void vulkan_execute_begin_command_buffer(struct vulkan_tutorial_info &info) {
    VkCommandBufferBeginInfo cmd_buf_info = {};
    cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buf_info.pNext = NULL;
    cmd_buf_info.flags = 0;
    cmd_buf_info.pInheritanceInfo = NULL;

    VkResult res = vkBeginCommandBuffer(info.cmd, &cmd_buf_info);
    assert(res == VK_SUCCESS);
}


void vulkan_acquire_next_image(struct vulkan_tutorial_info &info, VkSemaphore &imageAcquiredSemaphore) {

    VkResult res;
    VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
    imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    imageAcquiredSemaphoreCreateInfo.pNext = nullptr;
    imageAcquiredSemaphoreCreateInfo.flags = 0;

    res = vkCreateSemaphore(info.device, &imageAcquiredSemaphoreCreateInfo, nullptr, &imageAcquiredSemaphore);

    assert(res == VK_SUCCESS);

    res = vkAcquireNextImageKHR(info.device, info.swap_chain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE,
                                &info.current_buffer);

    assert(res == VK_SUCCESS);

}


void vulkan_build_command(struct vulkan_tutorial_info &info) {
    VkResult res;
    VkClearValue clearValue[2];
    clearValue[0].color.float32[0] = 1.0f;
    clearValue[0].color.float32[1] = 0.2f;
    clearValue[0].color.float32[2] = 0.2f;
    clearValue[0].color.float32[3] = 0.2f;
    clearValue[1].depthStencil.depth = 1.0f;
    clearValue[1].depthStencil.stencil = 0;

    VkRenderPassBeginInfo rp_begin;
    rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_begin.pNext = nullptr;
    rp_begin.renderPass = info.render_pass;
    rp_begin.framebuffer = info.framebuffers[info.current_buffer];
    rp_begin.renderArea.offset.x = 0;
    rp_begin.renderArea.offset.y = 0;
    rp_begin.renderArea.extent.width = info.width;
    rp_begin.renderArea.extent.height = info.height;
    rp_begin.clearValueCount = 2;
    rp_begin.pClearValues = clearValue;

    vkCmdBeginRenderPass(info.cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

    const VkDeviceSize offsets[1] = {0};

    vkCmdEndRenderPass(info.cmd);

    res = vkEndCommandBuffer(info.cmd);

    assert(res == VK_SUCCESS);

}


void vulkan_submit_command(struct vulkan_tutorial_info &info, VkFence &drawFence, VkSemaphore imageAcquiredSemaphore) {
    VkResult res;

    const VkCommandBuffer cmd_bufs[] = {info.cmd};

    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, nullptr, &drawFence);

    VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo[1] = {};
    submitInfo[0].pNext = nullptr;
    submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo[0].waitSemaphoreCount = 1;
    submitInfo[0].pWaitSemaphores = &imageAcquiredSemaphore;
    submitInfo[0].pWaitDstStageMask = &pipe_stage_flags;
    submitInfo[0].commandBufferCount = 1;
    submitInfo[0].pCommandBuffers = cmd_bufs;
    submitInfo[0].signalSemaphoreCount = 0;
    submitInfo[0].pSignalSemaphores = nullptr;

    res = vkQueueSubmit(info.graphics_queue, 1, submitInfo, drawFence);

    assert(res == VK_SUCCESS);

}

void vulkan_present(struct vulkan_tutorial_info &info, VkFence &drawFence) {
    VkResult res;

    VkPresentInfoKHR present;

    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = nullptr;
    present.swapchainCount = 1;
    present.pSwapchains = &info.swap_chain;
    present.pImageIndices = &info.current_buffer;
    present.pWaitSemaphores = nullptr;
    present.waitSemaphoreCount = 0;
    present.pResults = nullptr;

    do {
        res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);

    assert(res == VK_SUCCESS);
    res = vkQueuePresentKHR(info.present_queue, &present);
    assert(res == VK_SUCCESS);
}


void vulkan_wait_second(int seconds) {
    sleep(seconds);
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


void vulkan_init_instance_extension_name(struct vulkan_tutorial_info &info) {
    info.instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    info.instance_extension_names.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
}

void vulkan_init_device_extension_name(struct vulkan_tutorial_info &info) {
    info.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}


void vulkan_init_descriptor_set_layout_and_pipeline_layout(struct vulkan_tutorial_info &info, bool use_texture) {
    VkDescriptorSetLayoutBinding layoutBinding[2];
    layoutBinding[0].binding = 0;
    layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding[0].descriptorCount = 1;
    layoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding[0].pImmutableSamplers = nullptr;

    if (use_texture) {
        layoutBinding[1].binding = 1;
        layoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBinding[1].descriptorCount = 1;
        layoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layoutBinding[1].pImmutableSamplers = nullptr;
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.bindingCount = use_texture ? 2 : 1;
    descriptorSetLayoutCreateInfo.pBindings = layoutBinding;

    VkResult res;

    info.desc_layout.resize(NUM_DESCRIPTOR_SETS);
    res = vkCreateDescriptorSetLayout(info.device, &descriptorSetLayoutCreateInfo, nullptr, info.desc_layout.data());
    assert(res == VK_SUCCESS);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pipelineLayoutCreateInfo.pSetLayouts = info.desc_layout.data();

    res = vkCreatePipelineLayout(info.device, &pipelineLayoutCreateInfo, nullptr, &info.pipeline_layout);
    assert(res == VK_SUCCESS);
}


void vulkan_init_descriptor_pool(struct vulkan_tutorial_info &info, bool use_texture) {
    VkResult res;
    VkDescriptorPoolSize type_count[2];
    type_count[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    type_count[0].descriptorCount = 1;
    if (use_texture) {
        type_count[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        type_count[1].descriptorCount = 1;
    }

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.maxSets = 1;
    descriptorPoolCreateInfo.poolSizeCount = use_texture ? 2 : 1;
    descriptorPoolCreateInfo.pPoolSizes = type_count;

    res = vkCreateDescriptorPool(info.device, &descriptorPoolCreateInfo, nullptr, &info.desc_pool);

    assert(res == VK_SUCCESS);
}

void vulkan_init_descriptor_set(struct vulkan_tutorial_info &info, bool use_texture) {
    VkResult res;

    VkDescriptorSetAllocateInfo allocateInfo[1];
    allocateInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo[0].pNext = nullptr;
    allocateInfo[0].descriptorPool = info.desc_pool;
    allocateInfo[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
    allocateInfo[0].pSetLayouts = info.desc_layout.data();

    info.desc_set.resize(NUM_DESCRIPTOR_SETS);
    res = vkAllocateDescriptorSets(info.device, allocateInfo, info.desc_set.data());
    assert(res == VK_SUCCESS);

    VkWriteDescriptorSet writes[2];
    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = nullptr;
    writes[0].dstSet = info.desc_set[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &info.uniform_data.buffer_info;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    if (use_texture) {
        writes[0] = {};
        writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[0].pNext = nullptr;
        writes[0].dstSet = info.desc_set[0];
        writes[0].descriptorCount = 1;
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[0].pImageInfo = &info.texture_data.image_info;
        writes[0].dstArrayElement = 0;
        writes[0].dstBinding = 1;
    }

    vkUpdateDescriptorSets(info.device, use_texture ? 2 : 1, writes, 0, nullptr);
}

void vulkan_init_shader(struct vulkan_tutorial_info &info, const char *vertShaderText, const char *fragShaderText) {
    VkResult res;
    bool retVal;

    if (!(vertShaderText || fragShaderText)) return;

    VkShaderModuleCreateInfo moduleCreateInfo;

    if (vertShaderText) {
        std::vector<unsigned int> vtx_spv;
        info.shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.shaderStages[0].pNext = nullptr;
        info.shaderStages[0].pSpecializationInfo = nullptr;
        info.shaderStages[0].flags = 0;
        info.shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        info.shaderStages[0].pName = "main";

        retVal = vulkan_glsl_to_spv(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, vtx_spv);
        assert(retVal);

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = nullptr;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
        moduleCreateInfo.pCode = vtx_spv.data();
        res = vkCreateShaderModule(info.device, &moduleCreateInfo, nullptr, &info.shaderStages[0].module);
        assert(res == VK_SUCCESS);
    }

    if (fragShaderText) {
        std::vector<unsigned int> frag_spv;
        info.shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.shaderStages[1].pNext = nullptr;
        info.shaderStages[1].pSpecializationInfo = nullptr;
        info.shaderStages[1].flags = 0;
        info.shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        info.shaderStages[1].pName = "main";

        retVal = vulkan_glsl_to_spv(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText, frag_spv);

        assert(retVal);

        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.pNext = nullptr;
        moduleCreateInfo.flags = 0;
        moduleCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);
        moduleCreateInfo.pCode = frag_spv.data();
        res = vkCreateShaderModule(info.device, &moduleCreateInfo, nullptr, &info.shaderStages[1].module);
        assert(res == VK_SUCCESS);
    }
}


bool
vulkan_glsl_to_spv(const VkShaderStageFlagBits shader_type, const char *pshader, std::vector<unsigned int> &spirv) {
    shaderc::Compiler compiler;
    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(pshader, strlen(pshader),
                                                                     MapShadercType(shader_type), "shader");
    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        LOGE("Error: Id=%d, Msg=%s", module.GetCompilationStatus(), module.GetErrorMessage().c_str());
        return false;
    }
    spirv.assign(module.cbegin(), module.cend());
    return true;
}

void vulkan_init_pipeline_cache(struct vulkan_tutorial_info &info) {
    VkResult res;

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo;
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheCreateInfo.pNext = nullptr;
    pipelineCacheCreateInfo.initialDataSize = 0;
    pipelineCacheCreateInfo.pInitialData = nullptr;
    pipelineCacheCreateInfo.flags = 0;

    res = vkCreatePipelineCache(info.device, &pipelineCacheCreateInfo, nullptr, &info.pipelineCache);
    assert(res == VK_SUCCESS);
}


void vulkan_init_pipeline(struct vulkan_tutorial_info &info, VkBool32 include_vi) {
    VkResult res;

    VkDynamicState dynamicState[VK_DYNAMIC_STATE_RANGE_SIZE];
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    memset(dynamicState, 0, sizeof(dynamicState));

    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pNext = nullptr;
    dynamicStateCreateInfo.pDynamicStates = dynamicState;
    dynamicStateCreateInfo.dynamicStateCount = 0;

    // 顶点输入状态
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    memset(&vertexInputStateCreateInfo, 0, sizeof(vertexInputStateCreateInfo));

    if (include_vi) {
        vertexInputStateCreateInfo.pNext = nullptr;
        vertexInputStateCreateInfo.flags = 0;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = &info.vi_binding;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = info.vi_attribs;
    }

    // 绘制方式
    VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo;
    vkPipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    vkPipelineInputAssemblyStateCreateInfo.pNext = nullptr;
    vkPipelineInputAssemblyStateCreateInfo.flags = 0;
    vkPipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    vkPipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // 光栅化阶段
    VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo;
    vkPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    vkPipelineRasterizationStateCreateInfo.pNext = nullptr;
    vkPipelineRasterizationStateCreateInfo.flags = 0;
    vkPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    vkPipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    vkPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    vkPipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    vkPipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    vkPipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    vkPipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0;
    vkPipelineRasterizationStateCreateInfo.depthBiasClamp = 0;
    vkPipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0;
    vkPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;

    // 颜色混合
    VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo;
    vkPipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    vkPipelineColorBlendStateCreateInfo.flags = 0;
    vkPipelineColorBlendStateCreateInfo.pNext = nullptr;
    VkPipelineColorBlendAttachmentState attachmentState[1];
    attachmentState[0].colorWriteMask = 0xf;
    attachmentState[0].blendEnable = VK_FALSE;
    attachmentState[0].alphaBlendOp = VK_BLEND_OP_ADD;
    attachmentState[0].colorBlendOp = VK_BLEND_OP_ADD;
    attachmentState[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachmentState[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachmentState[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachmentState[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    vkPipelineColorBlendStateCreateInfo.attachmentCount = 1;
    vkPipelineColorBlendStateCreateInfo.pAttachments = attachmentState;
    vkPipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    vkPipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
    vkPipelineColorBlendStateCreateInfo.blendConstants[0] = 1.0f;
    vkPipelineColorBlendStateCreateInfo.blendConstants[1] = 1.0f;
    vkPipelineColorBlendStateCreateInfo.blendConstants[2] = 1.0f;
    vkPipelineColorBlendStateCreateInfo.blendConstants[3] = 1.0f;

    //设置 viewport
    VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo = {};
    vkPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vkPipelineViewportStateCreateInfo.pNext = nullptr;
    vkPipelineViewportStateCreateInfo.flags = 0;

    VkViewport vkviewport;
    vkviewport.minDepth = 0.0f;
    vkviewport.maxDepth = 1.0f;
    vkviewport.x = 0;
    vkviewport.y = 0;
    vkviewport.width = info.width;
    vkviewport.height = info.height;

    VkRect2D scissor;
    scissor.extent.width = info.width;
    scissor.extent.height = info.height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkPipelineViewportStateCreateInfo.viewportCount = NUM_VIEWPORTS;
    vkPipelineViewportStateCreateInfo.scissorCount = NUM_SCISSORS;
    vkPipelineViewportStateCreateInfo.pScissors = &scissor;
    vkPipelineViewportStateCreateInfo.pViewports = &vkviewport;

    // 设置深度和模板
    VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo;
    vkPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    vkPipelineDepthStencilStateCreateInfo.pNext = nullptr;
    vkPipelineDepthStencilStateCreateInfo.flags = 0;
    vkPipelineDepthStencilStateCreateInfo.depthTestEnable = false;
    vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = false;
    vkPipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    vkPipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    vkPipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    vkPipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    vkPipelineDepthStencilStateCreateInfo.back.compareMask = 0;
    vkPipelineDepthStencilStateCreateInfo.back.reference = 0;
    vkPipelineDepthStencilStateCreateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
    vkPipelineDepthStencilStateCreateInfo.back.writeMask = 0;
    vkPipelineDepthStencilStateCreateInfo.minDepthBounds = 0;
    vkPipelineDepthStencilStateCreateInfo.maxDepthBounds = 0;
    vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    vkPipelineDepthStencilStateCreateInfo.front = vkPipelineDepthStencilStateCreateInfo.back;

    // 多重采样
    VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo;
    vkPipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    vkPipelineMultisampleStateCreateInfo.pNext = nullptr;
    vkPipelineMultisampleStateCreateInfo.flags = 0;
    vkPipelineMultisampleStateCreateInfo.pSampleMask = nullptr;
    vkPipelineMultisampleStateCreateInfo.rasterizationSamples = NUM_SAMPLES;
    vkPipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    vkPipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    vkPipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
    vkPipelineMultisampleStateCreateInfo.minSampleShading = 0.0;

    // 渲染管线
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo;
    vkGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    vkGraphicsPipelineCreateInfo.pNext = nullptr;
    vkGraphicsPipelineCreateInfo.layout = info.pipeline_layout;
    vkGraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    vkGraphicsPipelineCreateInfo.basePipelineIndex = 0;
    vkGraphicsPipelineCreateInfo.flags = 0;
    vkGraphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pInputAssemblyState = &vkPipelineInputAssemblyStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pRasterizationState = &vkPipelineRasterizationStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pColorBlendState = &vkPipelineColorBlendStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pTessellationState = nullptr;
    vkGraphicsPipelineCreateInfo.pMultisampleState = &vkPipelineMultisampleStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pViewportState = &vkPipelineViewportStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pStages = info.shaderStages;
    vkGraphicsPipelineCreateInfo.stageCount = 1;
    vkGraphicsPipelineCreateInfo.renderPass = info.render_pass;
    vkGraphicsPipelineCreateInfo.subpass = 0;

    res = vkCreateGraphicsPipelines(info.device, info.pipelineCache, 1, &vkGraphicsPipelineCreateInfo, nullptr,
                                    &info.pipeline);

    assert(res == VK_SUCCESS);
}

void
vulkan_init_vertex_buffer(struct vulkan_tutorial_info &info, const void *vertexData, uint32_t dataSize,
                          uint32_t dataStride,
                          bool use_texture) {
    VkResult res;
    bool pass;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.size = dataSize;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.flags = 0;

    res = vkCreateBuffer(info.device, &bufferCreateInfo, nullptr, &info.vertex_buffer.buf);

    assert(res == VK_SUCCESS);

    VkMemoryRequirements memoryRequirements;

    vkGetBufferMemoryRequirements(info.device, info.vertex_buffer.buf, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};

    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.memoryTypeIndex = 0;
    allocateInfo.allocationSize = memoryRequirements.size;

    pass = memory_type_from_properties(info, memoryRequirements.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       &allocateInfo.memoryTypeIndex);

    assert(pass && "No mappable, coherent memory");

    res = vkAllocateMemory(info.device, &allocateInfo, nullptr, &(info.vertex_buffer.mem));

    assert(res == VK_SUCCESS);

    info.vertex_buffer.buffer_info.range = memoryRequirements.size;
    info.vertex_buffer.buffer_info.offset = 0;

    uint8_t *pData;
    res = vkMapMemory(info.device, info.vertex_buffer.mem, 0, memoryRequirements.size, 0, (void **) &pData);

    assert(res == VK_SUCCESS);

    memcpy(pData, vertexData, dataSize);

    vkUnmapMemory(info.device, info.vertex_buffer.mem);

    res = vkBindBufferMemory(info.device, info.vertex_buffer.buf, info.vertex_buffer.mem, 0);
    assert(res == VK_SUCCESS);

    info.vi_binding.binding = 0;
    info.vi_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    info.vi_binding.stride = dataStride;

    info.vi_attribs[0].binding = 0;
    info.vi_attribs[0].location = 0;
    info.vi_attribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    info.vi_attribs[0].offset = 0;
    info.vi_attribs[1].binding = 0;
    info.vi_attribs[1].location = 1;
    info.vi_attribs[1].format = use_texture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
    info.vi_attribs[1].offset = 16;
}

void vulkan_init_uniform_buffer(struct vulkan_tutorial_info &info) {
    VkResult res;
    bool pass;
    float fov = glm::radians(45.0f);
    if (info.width > info.height) {
        fov *= static_cast<float>(info.height) / static_cast<float>(info.width);
    }
    info.Projection = glm::perspective(fov, static_cast<float>(info.width) / static_cast<float>(info.height), 0.1f, 100.0f);


}
