package com.glumes.command_buffer_and_fence;

public class VulkanTutorial {

    static {
        System.loadLibrary("vulkan_tutorial");
    }

    public static native void run();

    public static native void destroy();
}
