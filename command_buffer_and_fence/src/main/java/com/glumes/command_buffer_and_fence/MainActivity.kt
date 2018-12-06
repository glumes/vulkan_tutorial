package com.glumes.command_buffer_and_fence

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        button.setOnClickListener {
            VulkanTutorial.run()
        }
    }

    override fun onPause() {
        super.onPause()
        VulkanTutorial.destroy()
    }

}
