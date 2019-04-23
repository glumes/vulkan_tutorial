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


