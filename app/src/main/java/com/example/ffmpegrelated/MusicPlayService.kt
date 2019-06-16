package com.example.ffmpegrelated

import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioTrack
import android.os.Build
import android.support.annotation.RequiresApi

class MusicPlayService {

    lateinit var audioTrack: AudioTrack

    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    fun createTrack(sampleRateInHz: Int, nb_channels: Int) {
        var channelConfig: Int
        channelConfig = when (nb_channels) {
            1 -> AudioFormat.CHANNEL_OUT_MONO
            2 -> AudioFormat.CHANNEL_IN_STEREO
            else -> AudioFormat.CHANNEL_OUT_MONO
        }

        val buffersize = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, AudioFormat.ENCODING_PCM_16BIT)
        audioTrack= AudioTrack(AudioManager.STREAM_MUSIC, sampleRateInHz, channelConfig, AudioFormat.ENCODING_PCM_16BIT, buffersize, AudioTrack.MODE_STREAM)
        audioTrack.play()
    }

    fun playTrack(buffer: ByteArray, length: Int){
        if(audioTrack?.playState==AudioTrack.PLAYSTATE_PLAYING){
            audioTrack.write(buffer, 0, length)
        }
    }


    external fun playSound(input: String)

    companion object{
        init {
            System.loadLibrary("HQPlayer")
        }
    }

}