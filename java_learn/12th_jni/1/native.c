#include <stdio.h>
#include "Jni.h"

//隐式建立:函数名必须固定格式,参考生成的Jni.h
void Java_Jni_hello_1implicit(JNIEnv *env, jobject cls)
{
    printf("hello java, I am from C language(implicit)\n");
}

//显式建立,函数名自定义
void c_hello(JNIEnv *env, jobject cls)
{
    printf("hello java, I am from C language(explicit)\n");
}

static const JNINativeMethod methods[] = {
    {"hello_explicit", "()V", (void *)c_hello},  //Java里调用的函数名;JNI字段描述符(参数、返回值);C语言实现的本地函数
};

//一旦Java调用System.loadLibrary,就会先调用JNI_OnLoad
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    JNIEnv *env;
    jclass cls;
    
    //根据版本获得env,为后面提供函数
    if ((*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_1_4)) 
        return JNI_ERR; 
    
    //查获取调用本程序的类
    cls = (*env)->FindClass(env, "Jni"); 
    if (cls == NULL) 
        return JNI_ERR;

    //使用RegisterNatives将C和Java建立联系
    if ((*env)->RegisterNatives(env, cls, methods, sizeof(methods)/sizeof(methods[0])) < 0)
        return JNI_ERR;

    return JNI_VERSION_1_4;
}