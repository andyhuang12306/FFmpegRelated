cmake_minimum_required(VERSION 3.4.1)


file(GLOB SOURCE src/main/cpp/*.cpp)
add_library( # Sets the name of the library.
        HQPlayer
        SHARED
        ${SOURCE})

include_directories(src/main/cpp/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L$-L${CMAKE_SOURCE_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}")

find_library( # Sets the name of the path variable.
        log-lib
        log)

target_link_libraries( # Specifies the target library.
        HQPlayer
        avfilter avformat avcodec avutil swresample swscale
        android
        z
        OpenSLES
        ${log-lib})