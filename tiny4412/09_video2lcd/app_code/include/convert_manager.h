
#ifndef _CONVERT_MANAGER_H
#define _CONVERT_MANAGER_H

#include <config.h>
#include <video_manager.h>

typedef struct video_convert
{
    char *name;
    int (*judge_support)(int pixel_format_in, int pixel_format_out);
    int (*convert)(p_video_buffer video_buf_in, p_video_buffer video_buf_out);
    int (*convert_exit)(p_video_buffer video_buf_out);
    struct video_convert *p_next;
} video_convert, *p_video_convert;


int register_video_convert(p_video_convert p_video_conv);
void show_video_convert_ops(void);
p_video_convert get_video_convert_ops(char *name);
p_video_convert get_video_convert_format(int pixel_format_in, int pixel_format_out);
int video_convert_init(void);

int yuv2rgb_init(void);
int mjpeg2rgb_init(void);
int rgb2rgb_init(void);


#endif /* _CONVERT_MANAGER_H */


