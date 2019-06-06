package com.example.ffmpegrelated

import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class SurfaceViewManager : SurfaceHolder.Callback{
    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {


    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
            mSurfaceViewHolder= holder!!
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {

    }

    companion object {
        init {
            System.loadLibrary("HQPlayer")
        }
    }

    private lateinit var mSurfaceViewHolder: SurfaceHolder
    private lateinit var mPath: String

    fun setSurfaceView(sf: SurfaceView){
        mSurfaceViewHolder=sf.holder
        if(null!=mSurfaceViewHolder){
            mSurfaceViewHolder.removeCallback(this)
        }
        mSurfaceViewHolder.addCallback(this)

    }

    fun setPath(p: String){
        mPath=p
    }

    fun startVideo(){
        native_start_video(mSurfaceViewHolder.surface, mPath)
    }

    external fun native_start_video(sf: Surface, path: String)
}