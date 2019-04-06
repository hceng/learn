#define LOG_TAG "LedService"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <utils/misc.h>
#include <utils/Log.h>

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <hardware/led_hal.h>

namespace android
{
    static led_device_t* led_device;

    jint ledOpen(JNIEnv *env, jobject cls)
    {
        jint err;
        hw_module_t* module;
        hw_device_t* device;

        ALOGI("native ledOpen");

        //1. hw_get_module for get module
        err = hw_get_module("led", (hw_module_t const**)&module);
        if (err == 0) {
            //2. module->methods->open for get device 
            err = module->methods->open(module, NULL, &device);
            
            if (err == 0) {
                //3. conversion, call led_open
                led_device = (led_device_t *)device;
                return led_device->led_open(led_device);
            } else {
                return -1;
            }
        }
        return -1;
    }

    void ledClose(JNIEnv *env, jobject cls)
    {
        ALOGI("nativeled Close");
        
        return;
    }

    jint ledCtrl(JNIEnv *env, jobject cls, jint number, jint status)
    {
        ALOGI("native ledCtrl %d, %d", number, status);
        
        return led_device->led_ctrl(led_device, number, status);
    }

    static const JNINativeMethod method_table[] = {
        {"native_ledOpen",  "()I",   (void *)ledOpen}, 
        {"native_ledClose", "()V",   (void *)ledClose}, 
        {"native_ledCtrl",  "(II)I", (void *)ledCtrl}, 
    };

    int register_android_server_LedService(JNIEnv *env)
    {
        return jniRegisterNativeMethods(env, "com/android/server/LedService",
                method_table, NELEM(method_table));
    }

};
