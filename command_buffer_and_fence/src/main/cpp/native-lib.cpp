#include <jni.h>
#include <string>

#include <vulkan_util.h>
#include "command_buffer_and_fence.h"

struct vulkan_tutorial_info info = {};

extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_command_1buffer_1and_1fence_VulkanTutorial_run(JNIEnv *env, jclass type) {
    run(info);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_glumes_command_1buffer_1and_1fence_VulkanTutorial_destroy(JNIEnv *env, jclass type) {

    destroy(info);
}