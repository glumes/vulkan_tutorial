//
// Created by glumes on 2018/11/6.
//

#ifndef VULKAN_TUTORIAL_LOGUTIL_H
#define VULKAN_TUTORIAL_LOGUTIL_H

#include <android/log.h>

#define LOG_TAG "VulkanTutorial"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#endif //VULKAN_TUTORIAL_LOGUTIL_H
