LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# LOCAL_MODULE := lights.default
LOCAL_MODULE := lights.tiny4412

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_C_INCLUDES := hardware/libhardware
LOCAL_SRC_FILES := lights.c
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)
