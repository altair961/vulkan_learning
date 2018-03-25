cmake_minimum_required(VERSION 3.4.1)




set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)

include_directories(${APP_GLUE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${ANDROID_NDK}/sources/third_party/vulkan/src/common/)

add_library( app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)

add_library(native-lib SHARED
            src/main/cpp/main.cpp
            ${ANDROID_NDK}/sources/third_party/vulkan/src/common/vulkan_wrapper.cpp)

find_library( log-lib
              log )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++14 \
                     -DVK_USE_PLATFORM_ANDROID_KHR")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

target_link_libraries( native-lib
                       app-glue
                       android
                       ${log-lib} )