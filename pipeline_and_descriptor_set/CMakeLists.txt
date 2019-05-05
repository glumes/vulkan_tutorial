# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp/glm)

include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/glm/)

add_library( # Sets the name of the library.
        vulkan_tutorial

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/native-lib.cpp
        src/main/cpp/pipeline_and_descriptor_set.cpp

        src/main/cpp/utils/vulkan_util.cpp
        src/main/cpp/utils/shader_util.cpp
        ${CMAKE_SOURCE_DIR}/src/main/cpp/vulkan_wrapper/vulkan_wrapper.cpp
        )

target_include_directories(vulkan_tutorial PRIVATE
        ${CMAKE_SOURCE_DIR}/../data
        ${SHADERC_SRC}/third_party/spirv-tools/include
        ${SHADERC_SRC}/third_party/spirv-tools/include/spirv-tools
        ${CMAKE_SOURCE_DIR}/src/main/cpp/vulkan_wrapper

        ${CMAKE_SOURCE_DIR}/src/main/cpp/common
        ${CMAKE_SOURCE_DIR}/src/main/cpp/utils

        ${ANDROID_NDK}/sources/android/native_app_glue
        # include 静态文件
        ${ANDROID_NDK}/sources/third_party/shaderc/include
        )


# SHADERC_SRC 变量指定为路径的绝对值
get_filename_component(SHADERC_SRC
        ${ANDROID_NDK}/sources/third_party/shaderc
        ABSOLUTE)

add_library(shaderc_lib STATIC IMPORTED)
set_target_properties(shaderc_lib PROPERTIES IMPORTED_LOCATION
        ${SHADERC_SRC}/libs/${ANDROID_STL}/${ANDROID_ABI}/libshaderc.a)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

#  使用 surface 拓展，需要加上这一句 关键
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -Werror -D VK_USE_PLATFORM_ANDROID_KHR")

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries( # Specifies the target library.
        vulkan_tutorial
        shaderc_lib
        android
        glm_shared
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})