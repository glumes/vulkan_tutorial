package com.glumes.tutorial_1;

/**
 * Created by glumes on 06/11/2018
 */
public class VulkanTutorial {

    static {
        System.loadLibrary("tutorial_1");
    }

    public static native void initInstance();

    public static native void initDevice();
}
