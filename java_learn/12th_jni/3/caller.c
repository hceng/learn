#include <stdio.h>  
#include <jni.h> 

/* create java virtual machine*/
jint create_vm(JavaVM** jvm, JNIEnv** env) 
{  
    JavaVMInitArgs args;  
    JavaVMOption options[1];  
    args.version = JNI_VERSION_1_6;  
    args.nOptions = 1;  
    options[0].optionString = "-Djava.class.path=./";  
    args.options = options;  
    args.ignoreUnrecognized = JNI_FALSE;  
    return JNI_CreateJavaVM(jvm, (void **)env, &args);  
}  

int main(int argc, char **argv)
{
    int r;
    int ret = 0;
    
    JavaVM* jvm;
    JNIEnv* env;
    jclass cls;
    
    jfieldID nameID, ageID;
    jmethodID mid, cid;
    
    jobject jobj;
    jstring jstr;
    
    /* 1. create java virtual machine */
    if (create_vm(&jvm, &env)) {
        printf("can not create jvm\n");
        return -1;
    }

    /* 2. get class */
    cls = (*env)->FindClass(env, "Hello");
    if (cls == NULL) {
        printf("can not find hello class\n");
        ret = -1;
        goto destroy;
    }
    
/*----------------Non-generic part------------------*/
    
    /* 3.get/set field */
    // 3.1 get field id (GetFieldID, GetStaticFieldID)
    // 3.2 get/set field (Get<Type>Field,GetStatic<Type>Field / Set<Type>Field,SetStatic<Type>Field)

    //java: private static String name;
    nameID = (*env)->GetStaticFieldID(env, cls, "name", "Ljava/lang/String;"); //3.1
    if (nameID == NULL) {
        ret = -1;
        printf("can not get field name\n"); //3.2
        goto destroy;
    }
    jstr = (*env)->NewStringUTF(env, "hceng");
    (*env)->SetStaticObjectField(env, jobj, nameID, jstr);

    //java: private static int age;
    ageID = (*env)->GetStaticFieldID(env, cls, "age", "I"); //3.1
    if (ageID == NULL) {
        ret = -1;
        printf("can not get field age\n");
        goto destroy;
    }
    (*env)->SetStaticIntField(env, jobj, ageID, 23); //3.2
    
    
/*Branch1:------for static method, No need create object-------*/
    /* 4. call method */
    // 4.1 get method id (GetMethodID, GetStaticMethodID)
    // 4.2 Preparation parameter
    // 4.3 call method (CallVoidMethod, CallStaticVoidMethod)
    mid = (*env)->GetStaticMethodID(env, cls, "main","([Ljava/lang/String;)V"); //4.1
    if (mid == NULL) {
        ret = -1;
        printf("can not get method\n");
        goto destroy;
    }
    (*env)->CallStaticVoidMethod(env, cls, mid, NULL); //4.3
    
/*Branch2:------for no static method, Need create object-------*/
    /* 4. create object  */
    // 4.1 get constructor method id (GetMethodID)
    // 4.2 create new object (NewObject)
    cid = (*env)->GetMethodID(env, cls, "<init>", "()V"); //4.1
    if (cid == NULL) {
        ret = -1;
        printf("can not get constructor method\n");
        goto destroy;
    }

    jobj = (*env)->NewObject(env, cls, cid); //4.2
    if (jobj == NULL) {
        ret = -1;
        printf("can not create object\n");
        goto destroy;
    }

    /* 5. call method */
    // 5.1 get method id (GetMethodID, GetStaticMethodID)
    // 5.2 Preparation parameter
    // 5.2 call method (CallVoidMethod, CallStaticVoidMethod)
    mid = (*env)->GetMethodID(env, cls, "typeData","(Ljava/lang/String;)I"); //5.1
    if (mid == NULL) {
        ret = -1;
        printf("can not get method\n");
        goto destroy;
    }
    jstr = (*env)->NewStringUTF(env, "www.hceng.cn"); //5.2
    r = (*env)->CallIntMethod(env, jobj, mid, jstr);  //5.3
    printf("%d\n", r);

    
destroy:
    (*jvm)->DestroyJavaVM(jvm);
    return ret;
}
