// Main.cpp
#include <android_native_app_glue.h>
#include <android/log.h>
////////////////////////////////////////////////////////////////////////////////
// The main entry into the game. 2018.03.19 DK
void android_main(
        android_app *pstDroidAPI) // The pointer's allocated memory helps manage the
                                  // interface between the Android OS and this game
{
// The function call prevents the linker from
// stripping out the native glue library
    app_dummy();
    for(uint8_t ui = 0; ui < 100; ++ui)
    {
// The function prints to log cat the index of the for loop
        __android_log_print(
                ANDROID_LOG_INFO,
                "Space Arcade",
                " -- SpaceArcade::android_main(), ui == %d \n",
                ui);
    } // End of for(uint8_t ui = 0; ui < 100; ++ui)
} // End of void android_main()
