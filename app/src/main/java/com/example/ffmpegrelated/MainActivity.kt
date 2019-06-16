package com.example.ffmpegrelated

import android.content.pm.PackageManager
import android.media.MediaPlayer
import android.os.Build
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.support.v4.app.ActivityCompat
import android.support.v4.content.ContextCompat
import android.util.Log
import android.view.SurfaceView
import android.view.View
import android.widget.ProgressBar
import android.widget.VideoView
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import java.io.IOException


class MainActivity : AppCompatActivity(), View.OnClickListener {

    private lateinit var mProgressBar: ProgressBar
    private lateinit var mSurfaceView: SurfaceView
    private lateinit var mManager: SurfaceViewManager

    private val permissions: ArrayList<String> = arrayListOf(
        android.Manifest.permission.READ_EXTERNAL_STORAGE,
        android.Manifest.permission.WRITE_EXTERNAL_STORAGE
    )

    private val permissionRequests: ArrayList<String> = arrayListOf()

    override fun onClick(v: View?) {
        when (v?.id) {
            R.id.tv_start -> mManager.startVideo()
            R.id.tv_start_audio -> MusicPlayService().playSound(mManager.getPath())
        }

    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        tv_start.setOnClickListener(this)
        tv_start_audio.setOnClickListener(this)
        mProgressBar = pb_progress
        mSurfaceView = sv_video

        checkPermi()

    }


    private fun checkPermi() {
        if (Build.VERSION.SDK_INT > 23) {
            for (i in 0 until permissions.size) {
                val checkSelfPermission = ContextCompat.checkSelfPermission(this, permissions[i])
                if (checkSelfPermission == PackageManager.PERMISSION_GRANTED) {
                    testFile()
                } else {
                    permissionRequests.add(permissions[i])
                }
            }

            if (permissionRequests.isNotEmpty()) {
                ActivityCompat.requestPermissions(this, permissionRequests.toArray(emptyArray()), 1000)
            }

        } else {
            testFile()
        }
    }

    private fun testFile() {
        val state = Environment.getExternalStorageState()
        if (state != Environment.MEDIA_MOUNTED) {
            return
        }
        val f =
            File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM).absolutePath + "/100ANDRO/MOV_0008.mp4")
        if (f.exists()) {
            mManager = SurfaceViewManager()
            mManager.setPath(f.absolutePath)
            mManager.setSurfaceView(mSurfaceView)
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        if (requestCode == 1000) {
            for (i in 0 until grantResults.size) {
                if (grantResults[i] == 0) {
                    testFile()
                }
            }
        }
    }


}
