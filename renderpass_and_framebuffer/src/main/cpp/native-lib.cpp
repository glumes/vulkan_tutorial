#include <jni.h>
#include <string>


#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <android/native_window.h>

#include "swapchain_and_surface.h"
#include <vulkan_util.h>

ANativeWindow *nativeWindow = nullptr;

struct vulkan_tutorial_info info = {};


extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_renderpass_1and_1framebuffer_VulkanTutorial_destroy(JNIEnv *env, jclass type) {

    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_renderpass_1and_1framebuffer_VulkanTutorial_run(JNIEnv *env, jclass type, jobject surface, jint width,
                                                                jint height) {

    // TODO

}