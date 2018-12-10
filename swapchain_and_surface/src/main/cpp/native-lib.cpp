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
Java_com_glumes_swapchain_1and_1surface_VulkanTutorial_run(JNIEnv *env, jclass type, jobject surface, jint width,
                                                           jint height) {

    nativeWindow = ANativeWindow_fromSurface(env, surface);

    run(info, nativeWindow, width, height);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_swapchain_1and_1surface_VulkanTutorial_destroy(JNIEnv *env, jclass type) {


}