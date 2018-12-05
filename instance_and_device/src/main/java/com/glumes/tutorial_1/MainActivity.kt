package com.glumes.tutorial_1

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        init_instance.setOnClickListener {
            initInstance()
        }

        init_device.setOnClickListener {
            initDevice()
        }
    }

    private fun initInstance() {
        VulkanTutorial.initInstance()
    }

    private fun initDevice() {
        VulkanTutorial.initDevice()
    }

}
