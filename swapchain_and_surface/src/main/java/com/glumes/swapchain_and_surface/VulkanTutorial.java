package com.glumes.swapchain_and_surface;

import android.view.Surface;

/**
 * Created by glumes on 10/12/2018
 */
public class VulkanTutorial {

    static {
        System.loadLibrary("vulkan_tutorial");
    }

    public static native void run(Surface surface, int width, int height);

    public static native void destroy();
}
