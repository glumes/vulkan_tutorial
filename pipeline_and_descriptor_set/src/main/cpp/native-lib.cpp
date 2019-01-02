#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_glumes_pipeline_1and_1descriptor_1set_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
