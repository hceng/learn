#include <stdio.h>
#include <jni.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <android/log.h>

#define DEBUG 1

#if(DEBUG==1)
#define LOG_TAG "JNI"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define LOGV(...) NULL
#define LOGD(...) NULL
#define LOGI(...) NULL
#define LOGE(...) NULL
#endif

static jint fd;

jint ledOpen(JNIEnv *env, jobject cls)
{
    LOGD("hardcontrol ledOpen\n");
	
	fd = open("/dev/leds", O_RDWR);
	if (fd >= 0)
		return 0;
	else
		return -1;
}

void ledClose(JNIEnv *env, jobject cls)
{
    LOGD("hardcontrol ledClose\n");
	
	close(fd);
	
	return;
}

jint ledCtrl(JNIEnv *env, jobject cls, jint number, jint status)
{
    LOGD("hardcontrol ledCtrl number=%d status=%d\n", number, status);
	
	int ret = ioctl(fd, status, number);
	
	return ret;
}

static const JNINativeMethod methods[] = {
    {"ledOpen",  "()I",   (void *)ledOpen}, 
	{"ledClose", "()V",   (void *)ledClose}, 
	{"ledCtrl",  "(II)I", (void *)ledCtrl}, 
};


JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    JNIEnv *env;
    jclass cls;
    
    if ((*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_1_4)) 
        return JNI_ERR; 
    
    cls = (*env)->FindClass(env, "cn/hceng/hardlibrary/HardControl"); 
    if (cls == NULL) 
        return JNI_ERR;

    if ((*env)->RegisterNatives(env, cls, methods, sizeof(methods)/sizeof(methods[0])) < 0)
        return JNI_ERR;
	
    return JNI_VERSION_1_4;
}