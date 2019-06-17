//
// Created by Huang Qi on 2019-06-16.
//
#include "../../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/android/log.h"
#include "../../../../../../../Library/Android/sdk/ndk-bundle/toolchains/llvm/prebuilt/darwin-x86_64/sysroot/usr/include/string.h"

#ifndef FFMPEGRELATED2_FFMPEGMUSIC_H
#define FFMPEGRELATED2_FFMPEGMUSIC_H

#endif //FFMPEGRELATED2_FFMPEGMUSIC_H


#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, "LC", FORMAT, ##__VA_ARGS__);
void createEngine();
void createMixVOlume();
void createPlayer(const char *path);
void releaseResource();


