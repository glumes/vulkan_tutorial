package com.glumes.renderpass_and_framebuffer

import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.SurfaceHolder
import kotlinx.android.synthetic.main.activity_main.*


class MainActivity : AppCompatActivity() {

    private var mWidth: Int = 0
    private var mHeight: Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)


        surfaceview.holder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
                mWidth = width
                mHeight = height
            }

            override fun surfaceDestroyed(holder: SurfaceHolder?) {
            }

            override fun surfaceCreated(holder: SurfaceHolder?) {

            }

        })

        button.setOnClickListener {
            VulkanTutorial.run(surfaceview.holder.surface, mWidth, mHeight)
        }
    }

    override fun onPause() {
        super.onPause()
        VulkanTutorial.destroy()
    }

}
