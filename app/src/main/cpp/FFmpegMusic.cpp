//
// Created by Huang Qi on 2019-06-16.
//
#include <SLES/OpenSLES.h>
#include "include/FFmpegMusic.h"
#include "../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/c++/v1/cstdint"

extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/frame.h"
#include "include/libswresample/swresample.h"
}


AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVCodec *pCodec;
AVPacket *packet;
AVFrame *frame;
SwrContext *swrContext;
uint8_t *out_buffer;
int out_channel_nb;
int audio_stream_idx=-1;

int createFFmpeg(int *rate, int *channel, const char *path) {
    av_register_all();
    pFormatCtx=avformat_alloc_context();
    int error;
    char buf[]="";
    if(error==avformat_open_input(&pFormatCtx, path, NULL, NULL)<0){
        av_strerror(error, buf, 1024);
        LOGE("Couldn't open file %s: %d(%s)", path, error, buf);

    }
    if(avformat_find_stream_info(pFormatCtx, NULL)<0){
        LOGE("%s", "get video info failed");
        return -1;
    }

    int i=0;
    for(int i=0; i<pFormatCtx->nb_streams; i++){
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            LOGE("audio id found: %d", pFormatCtx->streams[i]->codecpar->codec_type);
            audio_stream_idx=i;
            break;
        }
    }

    AVCodecParameters *codecP=pFormatCtx->streams[audio_stream_idx]->codecpar;
    pCodec=avcodec_find_decoder(codecP->codec_id);
    pCodecCtx=avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, codecP);
    avcodec_open2(pCodecCtx, pCodec, NULL);
    packet=av_packet_alloc();
    frame=av_frame_alloc();
    swrContext=swr_alloc();
    out_buffer=(uint8_t *)av_malloc(44100*2);
    uint64_t out_ch_layout=AV_CH_LAYOUT_STEREO;
    enum AVSampleFormat out_format=AV_SAMPLE_FMT_S16;
    int out_sample_rate=pCodecCtx->sample_rate;

    swr_alloc_set_opts(swrContext, out_ch_layout, out_format, out_sample_rate,
            pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0, NULL);
    swr_init(swrContext);
    out_channel_nb=av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    *rate=pCodecCtx->sample_rate;
    *channel=pCodecCtx->channels;
    return 0;

}


int getPcm(void **pcm, unsigned int *pcm_size){
    int frameCount=0;
    int got_frame;
    while(av_read_frame(pFormatCtx, packet)>=0){
        if(packet->stream_index==audio_stream_idx){
            avcodec_decode_audio4(pCodecCtx, frame, &got_frame, packet);
            if(got_frame){
                LOGE("decoding");
                swr_convert(swrContext, &out_buffer, 44100*2, (const uint8_t **)frame->data, frame->nb_samples);
                int size=av_samples_get_buffer_size(NULL, out_channel_nb, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
                *pcm=out_buffer;
                *pcm_size=size;
                break;
            }
        }
    }
    return 0;
}

void releaseFFmpeg(){
    av_free_packet(packet);
    av_free(out_buffer);
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}

