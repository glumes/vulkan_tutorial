//
// Created by glumes on 2018/12/6.
//

#ifndef VULKAN_TUTORIAL_SWAPCHAIN_AND_SURFACE_H
#define VULKAN_TUTORIAL_SWAPCHAIN_AND_SURFACE_H

#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <android/native_window.h>

#include <vulkan_util.h>

//
//#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
//    {                                                                          \
//        info.fp##entrypoint =                                                  \
//            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
//        if (info.fp##entrypoint == NULL) {                                     \
////            std::cout << "vkGetDeviceProcAddr failed to find vk" #entrypoint;  \
//            exit(-1);                                                          \
//        }                                                                      \
//    }

void run(struct vulkan_tutorial_info &info, ANativeWindow *window, int width, int height);


void destroy(struct vulkan_tutorial_info &info);

#endif //VULKAN_TUTORIAL_SWAPCHAIN_AND_SURFACE_H
