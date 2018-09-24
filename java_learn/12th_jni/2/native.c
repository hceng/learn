#include <stdio.h>
#include "Jni.h"
#include <stdlib.h>

#if 0
//隐式建立
jfloat Java_Jni_typeData1(JNIEnv *env, jclass cls, jint a)
{
    printf("C: get val: %d, will return (float)%d \n", a, a);
    return (float)a;
}

jstring Java_Jni_typeData2(JNIEnv *env, jclass cls, jstring str)
{
    const jbyte *cstr;
    cstr = (*env)->GetStringUTFChars(env, str, NULL);
    if (cstr == NULL) 
        return NULL; 
    
    printf("C: get str: %s, will return jack \n", cstr);
    (*env)->ReleaseStringUTFChars(env, str, cstr);
    
    return (*env)->NewStringUTF(env, "jack");
}

jintArray JNICALL Java_Jni_typeData3(JNIEnv *env, jclass cls, jintArray arr)
{
    jint *carr;
    jint *oarr;
    jintArray rarr;
    jint i, n = 0;
    
    carr = (*env)->GetIntArrayElements(env, arr, NULL);
    if (carr == NULL) 
        return 0; 

    n = (*env)->GetArrayLength(env, arr);
    
    printf("C: get number: ");
    for (i = 0; i < n; i++)     
        printf("%d ", carr[i]);
    printf(", will return opposite number\n");
    
    
    oarr = malloc(sizeof(jint) * n);
    if (oarr == NULL)
    {
        (*env)->ReleaseIntArrayElements(env, arr, carr, 0);
        return 0;
    }

    for (i = 0; i < n; i++)
        oarr[i] = carr[n-1-i];
    
    (*env)->ReleaseIntArrayElements(env, arr, carr, 0);

    /* create jintArray */
    rarr = (*env)->NewIntArray(env, n);
    if (rarr == NULL)
        return 0;

    (*env)->SetIntArrayRegion(env, rarr, 0, n, oarr);
    free(oarr);
    
    return rarr;
}


#else
    

//显式建立
jfloat JNICALL c_typeData1(JNIEnv *env, jclass cls, jint a)
{
    printf("C: get val: %d, will return (float)%d \n", a, a);
    return (float)a;
}

jstring JNICALL c_typeData2(JNIEnv *env, jclass cls, jstring str)
{
    const jbyte *cstr;
    cstr = (*env)->GetStringUTFChars(env, str, NULL);
    if (cstr == NULL) 
        return NULL; 
    
    printf("C: get str: %s, will return jack \n", cstr);
    (*env)->ReleaseStringUTFChars(env, str, cstr);
    
    return (*env)->NewStringUTF(env, "jack");
}

jintArray JNICALL c_typeData3(JNIEnv *env, jclass cls, jintArray arr)
{
    jint *carr;
    jint *oarr;
    jintArray rarr;
    jint i, n = 0;
    
    carr = (*env)->GetIntArrayElements(env, arr, NULL);
    if (carr == NULL) 
        return 0; 

    n = (*env)->GetArrayLength(env, arr);
    
    printf("C: get number: ");
    for (i = 0; i < n; i++)     
        printf("%d ", carr[i]);
    printf(", will return opposite number\n");
    
    
    oarr = malloc(sizeof(jint) * n);
    if (oarr == NULL)
    {
        (*env)->ReleaseIntArrayElements(env, arr, carr, 0);
        return 0;
    }

    for (i = 0; i < n; i++)
        oarr[i] = carr[n-1-i];
    
    (*env)->ReleaseIntArrayElements(env, arr, carr, 0);

    /* create jintArray */
    rarr = (*env)->NewIntArray(env, n);
    if (rarr == NULL)
        return 0;

    (*env)->SetIntArrayRegion(env, rarr, 0, n, oarr);
    free(oarr);
    
    return rarr;
}


static const JNINativeMethod methods[] = {
    {"typeData1", "(I)F", (void *)c_typeData1},  
    {"typeData2", "(Ljava/lang/String;)Ljava/lang/String;", (void *)c_typeData2}, 
    {"typeData3", "([I)[I", (void *)c_typeData3}, 
};


JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    JNIEnv *env;
    jclass cls;

    if ((*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_1_4)) 
        return JNI_ERR; 
    
    cls = (*env)->FindClass(env, "Jni"); 
    if (cls == NULL) 
        return JNI_ERR;

    if ((*env)->RegisterNatives(env, cls, methods, sizeof(methods)/sizeof(methods[0])) < 0)
        return JNI_ERR;

    return JNI_VERSION_1_4;
}

#endif