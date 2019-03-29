//
// Created by glumes on 2019/3/25.
//

#ifndef VULKAN_TUTORIAL_RENDERPASS_AND_FRAMEBUFFER_H
#define VULKAN_TUTORIAL_RENDERPASS_AND_FRAMEBUFFER_H


#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <android/native_window.h>

#include <vulkan_util.h>


void run(struct vulkan_tutorial_info &info, ANativeWindow *window, int width, int height);

void destroy(struct vulkan_tutorial_info &info);

#endif //VULKAN_TUTORIAL_RENDERPASS_AND_FRAMEBUFFER_H
