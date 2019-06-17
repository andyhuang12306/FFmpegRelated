package com.example.ffmpegrelated


class MusicPlay {

    external fun playSound(input: String)
    external fun stopSound(input: String)

    companion object{
        init {
            System.loadLibrary("HQPlayer")
        }
    }

}