#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <unistd.h>
#include <libavutil/channel_layout.h>
#include "FFmpegMusic.h"
#include "OpenSLEngine.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
#include <libavformat/avio.h>
#include <libswresample/swresample.h>
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ffmpegrelated_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(av_version_info());
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegrelated_SurfaceViewManager_native_1start_1video(
        JNIEnv *env,
        jobject instance, jobject surface, jstring path_) {

    const char *path=env->GetStringUTFChars(path_, 0);

    //init
    avformat_network_init();
    av_register_all();

    //get context
    AVFormatContext *formatContext=avformat_alloc_context();

    //open input with video path and context
    AVDictionary *opts=NULL;
    av_dict_set(&opts, "time_out", "30000000000", 0);
    int ret=avformat_open_input(&formatContext, path, NULL, NULL);

    if(ret){
        return;
    }
    //get stream info according to context
    avformat_find_stream_info(formatContext, NULL);

    int video_stream_index=-1;
    for(int i=0; i<formatContext->nb_streams; i++){
        if(formatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO){
            video_stream_index=i;
            break;
        }
    }

    //get video deco parameter according to video stream index
    AVCodecParameters *dedecpar=formatContext->streams[video_stream_index]->codecpar;

    //get video decoder according to id
    AVCodec *dec=avcodec_find_decoder(dedecpar->codec_id);

    //get decoder context
    AVCodecContext *codecContext=avcodec_alloc_context3(dec);

    //bind context and parameter together
    avcodec_parameters_to_context(codecContext, dedecpar);

    //bind context and decoder together
    avcodec_open2(codecContext, dec, NULL);

    //set packet already for later use
    AVPacket *packet=av_packet_alloc();

    //set stream dec way
    SwsContext *swsContext=sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
            codecContext->width, codecContext->height, AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0 ,0);

    ANativeWindow *nativeWindow=ANativeWindow_fromSurface(env, surface);
    ANativeWindow_Buffer outBuffer;

    ANativeWindow_setBuffersGeometry(nativeWindow, codecContext->width, codecContext->height, WINDOW_FORMAT_RGBA_8888);

    while(av_read_frame(formatContext, packet)>=0){
        avcodec_send_packet(codecContext, packet);
        AVFrame *frame=av_frame_alloc();
        ret=avcodec_receive_frame(codecContext, frame);
        if(ret==AVERROR(EAGAIN)){
            continue;
        }else if(ret<0){
            break;
        }

        uint8_t *dst_data[4];
        int dst_linesize[4];
        av_image_alloc(dst_data, dst_linesize, codecContext->width, codecContext->height, AV_PIX_FMT_RGBA, 1);

        //draw
        sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, dst_data, dst_linesize);

        ANativeWindow_lock(nativeWindow, &outBuffer, NULL);

        uint8_t *firstWindow= static_cast<uint8_t *>(outBuffer.bits);

        uint8_t *src_data=dst_data[0];
        int destStride=outBuffer.stride*4;
        int src_linesize=dst_linesize[0];
        for(int i=0; i<outBuffer.height; i++){
            memcpy(firstWindow+i*destStride, src_data+i*src_linesize, destStride);
        }

        ANativeWindow_unlockAndPost(nativeWindow);
        usleep(1000*6);
        av_frame_free(&frame);

    }

    env->ReleaseStringUTFChars(path_, path);

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegrelated_MusicPlayService_playSound(
        JNIEnv *env,
        jobject instance, jstring path_) {

//    const char *path=env->GetStringUTFChars(path_, 0);
//
//    //init
//    avformat_network_init();
//    av_register_all();
//
//    //get context
//    AVFormatContext *formatContext=avformat_alloc_context();
//
//    //open input with video path and context
//    AVDictionary *opts=NULL;
//    av_dict_set(&opts, "time_out", "30000000000", 0);
//    int ret=avformat_open_input(&formatContext, path, NULL, NULL);
//
//    if(ret){
//        return;
//    }
//    //get stream info according to context
//    avformat_find_stream_info(formatContext, NULL);
//
//    int audio_stream_index=-1;
//    for(int i=0; i<formatContext->nb_streams; i++){
//        if(formatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
//            audio_stream_index=i;
//            break;
//        }
//    }
//
//    //get video deco parameter according to video stream index
//    AVCodecParameters *dedecpar=formatContext->streams[audio_stream_index]->codecpar;
//
//    //get video decoder according to id
//    AVCodec *dec=avcodec_find_decoder(dedecpar->codec_id);
//
//    //get decoder context
//    AVCodecContext *codecContext=avcodec_alloc_context3(dec);
//
//    //bind context and parameter together
//    avcodec_parameters_to_context(codecContext, dedecpar);
//
//    //bind context and decoder together
//    avcodec_open2(codecContext, dec, NULL);
//
//    //set packet already for later use
//    AVPacket *packet=av_packet_alloc();
//
//    AVFrame *frame=av_frame_alloc();
//    SwrContext *swrContext=swr_alloc();
//    uint8_t *out_buffer=(uint8_t *)av_malloc(44100*2);
//    uint64_t out_ch_layout=AV_CH_LAYOUT_STEREO;
//    enum AVSampleFormat out_format=AV_SAMPLE_FMT_S16;
//    int out_sample_rate=codecContext->sample_rate;
//
//    swr_alloc_set_opts(swrContext, out_ch_layout, out_format, out_sample_rate,
//            codecContext->channel_layout, codecContext->sample_fmt, codecContext->sample_rate, 0, NULL);
//    swr_init(swrContext);
//    int out_channel_nb=av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
//    jclass music_player=env->GetObjectClass(instance);
//    jmethodID createTrack=env->GetMethodID(music_player, "createTrack", "(II)V");
//    env->CallVoidMethod(instance, createTrack, 44100, out_channel_nb);
//    jmethodID playTrack=env->GetMethodID(music_player, "playTrack", "([BI)V");
//    int got_frame;
//    while(av_read_frame(formatContext, packet)>=0){
//        if(packet->stream_index==audio_stream_index){
//            avcodec_decode_audio4(codecContext, frame, &got_frame, packet);
//            if(got_frame){
//                swr_convert(swrContext, &out_buffer, 44100*2, (const uint8_t **)frame->data, frame->nb_samples);
//                int size=av_samples_get_buffer_size(NULL, out_channel_nb, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
//                jbyteArray audio_sample_array=env->NewByteArray(size);
//                env->SetByteArrayRegion(audio_sample_array, 0, size, (const jbyte *)out_buffer);
//                env->CallVoidMethod(instance, playTrack, audio_sample_array, size);
////                env->DeleteGlobalRef(audio_sample_array);
//            }
//        }
//    }
//
//    av_frame_free(&frame);
//    swr_free(&swrContext);
//    avcodec_close(codecContext);
//    avformat_close_input(&formatContext);
//    env->ReleaseStringUTFChars(path_, path);

}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegrelated_MusicPlay_playSound(
        JNIEnv *env,
        jobject instance, jstring path_) {

    const char *path=env->GetStringUTFChars(path_, 0);
    createEngine();
    createMixVOlume();
    createPlayer(path);

}
extern "C" JNIEXPORT void JNICALL
Java_com_example_ffmpegrelated_MusicPlay_stopSound(
        JNIEnv *env,
        jobject instance, jstring path_) {

    const char *path=env->GetStringUTFChars(path_, 0);

    releaseResource();

}


