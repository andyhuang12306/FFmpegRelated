//
// Created by Huang Qi on 2019-06-16.
//

extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/frame.h"
#include "include/libswresample/swresample.h"
#include "include/libswscale/swscale.h"

#include "../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/SLES/OpenSLES.h"
#include "../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/SLES/OpenSLES_Android.h"
#include "../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/unistd.h"
#include "../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/android/native_window_jni.h"
}

#include "include/FFmpegMusic.h"

#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, "LC", FORMAT, ##__VA_ARGS__);


SLObjectItf engineObject=NULL;
SLEngineItf  engineEngine=NULL;

SLObjectItf outputMixObject=NULL;
SLEnvironmentalReverbItf outputMixEnvironmentalReverb=NULL;
SLEnvironmentalReverbSettings settings=SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;

SLObjectItf audioPlayer=NULL;
SLPlayItf slPlayItf=NULL;
SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf=NULL;

SLuint32 bufferSize=0;
void *buffer;
void getQueueCallBack(SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf, void* context){
    bufferSize=0;
    getPcm(&buffer, &bufferSize);
    if(buffer!=NULL&&bufferSize!=0){
        (*slAndroidSimpleBufferQueueItf)->Enqueue(slAndroidSimpleBufferQueueItf, buffer, bufferSize);
    }
}

void createEngine(){
    slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
}

void createMixVOlume(){
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    SLresult sLresult=(*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);

    if(SL_RESULT_SUCCESS==sLresult){
        (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb, &settings);
    }
}

void createPlayer(const char* path){
    int rate;
    int channel;
    createFFmpeg(&rate, &channel, path);
    LOGE("rate %d", rate);
    LOGE("channel %d", channel);
    SLDataLocator_AndroidBufferQueue android_queue={SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM, (SLuint32)channel, (SLuint32)(rate * 1000),
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                           SL_SPEAKER_FRONT_LEFT,
                            SL_BYTEORDER_LITTLEENDIAN};

    SLDataSource dataSource={&android_queue, &pcm};

    SLDataLocator_OutputMix slDataLocator_outputMix={SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink slDataSink={&slDataLocator_outputMix, NULL};
    const SLInterfaceID ids[3]={SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3]={SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &audioPlayer, &dataSource, &slDataSink, 3, ids,req);
    (*audioPlayer)->Realize(audioPlayer, SL_BOOLEAN_FALSE);
    (*audioPlayer)->GetInterface(audioPlayer, SL_IID_PLAY, &slPlayItf);
    (*audioPlayer)->GetInterface(audioPlayer, SL_IID_BUFFERQUEUE, &slAndroidSimpleBufferQueueItf);
    (*slAndroidSimpleBufferQueueItf)->RegisterCallback(slAndroidSimpleBufferQueueItf, getQueueCallBack, NULL);
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    getQueueCallBack(slAndroidSimpleBufferQueueItf, NULL);

}

void releaseResource(){
    if(audioPlayer!=NULL){
        (*audioPlayer)->Destroy(audioPlayer);
        audioPlayer=NULL;
        slAndroidSimpleBufferQueueItf=NULL;
        slPlayItf=NULL;
    }
    if(outputMixObject!=NULL){
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject=NULL;
        outputMixEnvironmentalReverb=NULL;
    }
    if(engineObject!=NULL){
        (*engineObject)->Destroy(engineObject);
        engineObject=NULL;
        engineEngine=NULL;
    }

    releaseFFmpeg();
}






