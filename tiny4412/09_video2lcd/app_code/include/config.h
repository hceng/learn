#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <linux/videodev2.h>

#define __DEBUG__  
#ifdef  __DEBUG__  
#define printf_debug(format,...) printf("File: "__FILE__", Line: %05d: "format"/n", __LINE__, ##__VA_ARGS__)  
#else  
#define printf_debug(format,...)  /* do nothing */
#endif


#define VIDEO_DEVICE_NAME  "/dev/video0"
#define FB_DEVICE_NAME     "/dev/fb0"

#endif