
#include <convert_manager.h>
#include <video_manager.h>
#include <string.h>
#include <stdlib.h>

static int is_support_rgb2rgb(int pixel_format_in, int pixel_format_out);
static int rgb2rgb_convert(p_video_buffer video_buf_in, p_video_buffer video_buf_out);
static int rgb2rgb_convert_exit(p_video_buffer video_buf_out);


static int is_support_rgb2rgb(int pixel_format_in, int pixel_format_out)
{

    if (pixel_format_in != V4L2_PIX_FMT_RGB565)
        return 0;
    if ((pixel_format_out != V4L2_PIX_FMT_RGB565) && (pixel_format_out != V4L2_PIX_FMT_RGB32))
        return 0;

    return 1;

}

static int rgb2rgb_convert(p_video_buffer video_buf_in, p_video_buffer video_buf_out)
{
    p_pixel_datas p_pixel_datas_in  = &video_buf_in->pixel_datas;
    p_pixel_datas p_pixel_datas_out = &video_buf_out->pixel_datas;

    int x, y;
    int r, g, b;
    int color;
    unsigned short *scr = (unsigned short *)p_pixel_datas_in->pixel_datas_addr;
    unsigned int *dest;

    if (video_buf_in->pixel_format != V4L2_PIX_FMT_RGB565)
        return -1;

    if (video_buf_out->pixel_format == V4L2_PIX_FMT_RGB565) //rgb565 -> rgb565
    {
        p_pixel_datas_out->width  = p_pixel_datas_in->width;
        p_pixel_datas_out->height = p_pixel_datas_in->height;
        p_pixel_datas_out->bpp    = 16;
        p_pixel_datas_out->line_bytes  = p_pixel_datas_out->width * p_pixel_datas_out->bpp / 8;
        p_pixel_datas_out->total_bytes = p_pixel_datas_out->line_bytes * p_pixel_datas_out->height;
        if (!p_pixel_datas_out->pixel_datas_addr)
        {
            p_pixel_datas_out->pixel_datas_addr = malloc(p_pixel_datas_out->total_bytes);
            if (!p_pixel_datas_out->pixel_datas_addr)
                return -1;
        }
        
        memcpy(p_pixel_datas_out->pixel_datas_addr, p_pixel_datas_in->pixel_datas_addr, p_pixel_datas_out->total_bytes);
        return 0;
    }
    else if (video_buf_out->pixel_format == V4L2_PIX_FMT_RGB32) //rgb565 -> rgb32
    {
        p_pixel_datas_out->width  = p_pixel_datas_in->width;
        p_pixel_datas_out->height = p_pixel_datas_in->height;
        p_pixel_datas_out->bpp    = 32;
        p_pixel_datas_out->line_bytes  = p_pixel_datas_out->width * p_pixel_datas_out->bpp / 8;
        p_pixel_datas_out->total_bytes = p_pixel_datas_out->line_bytes * p_pixel_datas_out->height;
        if (!p_pixel_datas_out->pixel_datas_addr)
        {
            p_pixel_datas_out->pixel_datas_addr = malloc(p_pixel_datas_out->total_bytes);
            if (!p_pixel_datas_out->pixel_datas_addr)
                return -1;
        }
        
        dest = (unsigned int *)p_pixel_datas_out->pixel_datas_addr;
        
        for (y = 0; y < p_pixel_datas_out->height; y++)
        {
            for (x = 0; x < p_pixel_datas_out->width; x++)
            {
                color = *scr++;
                //从RGB565格式的数据中提取出R,G,B 
                r = color >> 11;
                g = (color >> 5) & (0x3f);
                b = color & 0x1f;

                //把r,g,b转为0x00RRGGBB的32位数据
                color = ((r << 3) << 16) | ((g << 2) << 8) | (b << 3);

                *dest = color;
                dest++;
            }
        }
        return 0;
    }

    return -1;
}


static int rgb2rgb_convert_exit(p_video_buffer video_buf_out)
{
    if (video_buf_out->pixel_datas.pixel_datas_addr)
    {
        free(video_buf_out->pixel_datas.pixel_datas_addr);
        video_buf_out->pixel_datas.pixel_datas_addr = NULL;
    }
    return 0;
}


static video_convert rgb2rgb_convert_ops = {
    .name           = "rgb2rgb",   
    .judge_support  = is_support_rgb2rgb,
    .convert        = rgb2rgb_convert,  
    .convert_exit   = rgb2rgb_convert_exit,
};

int rgb2rgb_init(void)
{
    return register_video_convert(&rgb2rgb_convert_ops);
}


