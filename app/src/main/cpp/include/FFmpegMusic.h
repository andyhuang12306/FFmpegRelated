//
// Created by Huang Qi on 2019-06-16.
//
#include "../../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/android/log.h"
#include "../../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/string.h"
#include <SLES/OpenSLES.h>
#ifndef FFMPEGRELATED2_FFMPEGMUSIC_H
#define FFMPEGRELATED2_FFMPEGMUSIC_H

#endif //FFMPEGRELATED2_FFMPEGMUSIC_H


#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, "LC", FORMAT, ##__VA_ARGS__);
int createFFmpeg(int *rate, int *channel, const char *path);

int getPcm(void **pcm, unsigned int *pcm_size);

void releaseFFmpeg();


