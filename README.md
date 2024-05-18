# Vulkan
Getting familiar with Vulkan API on Android

The repository follows this tutorial: 
https://vulkan-tutorial.com/

The project is created from scratch in Android Studio on Ubuntu.

In order to run the application you need to install Android Studio, then from within Android Studio go to Tools -> Android -> SDK Manager -> SDK Tools, select CMake, LLDB and NDK, click apply.
After that create environment variable ANDROID_NDK. To do that go to your home directory, open a hidden file .profile and add the following line:
export ANDROID_NDK=~/Android/Sdk/ndk-bundle

Here Android is a folder where Android SDK is installed
