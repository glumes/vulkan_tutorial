//
// Created by glumes on 2018/12/11.
//

#ifndef VULKAN_TUTORIAL_PIPELINE_AND_DESCRIPTOR_SET_H
#define VULKAN_TUTORIAL_PIPELINE_AND_DESCRIPTOR_SET_H

#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <android/native_window.h>

#include <vulkan_util.h>

#define NUM_DESCRIPTOR_SETS 1


void run(struct vulkan_tutorial_info &info, ANativeWindow *window, int width, int height);

void destroy(struct vulkan_tutorial_info &info);

#endif //VULKAN_TUTORIAL_PIPELINE_AND_DESCRIPTOR_SET_H
