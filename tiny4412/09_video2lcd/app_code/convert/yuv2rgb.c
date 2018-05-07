
#include <convert_manager.h>
#include <video_manager.h>
#include <stdlib.h>
#include "color.h"

static int is_support_yuv2rgb(int pixel_format_in, int pixel_format_out);
static int yuv2rgb_convert(p_video_buffer video_buf_in, p_video_buffer video_buf_out);
static int yuv2rgb_convert_exit(p_video_buffer video_buf_out);


static int is_support_yuv2rgb(int pixel_format_in, int pixel_format_out)
{
    if (pixel_format_in != V4L2_PIX_FMT_YUYV)
        return 0;
    if ((pixel_format_out != V4L2_PIX_FMT_RGB565) && (pixel_format_out != V4L2_PIX_FMT_RGB32))
        return 0;

    return 1;
}


//refer:luvcview
//translate YUV422Packed to rgb565
static unsigned int yuv422torgb565(unsigned char * input_ptr, unsigned char * output_ptr, 
                                          unsigned int image_width, unsigned int image_height)
{
	unsigned int i, size;
	unsigned char Y, Y1, U, V;
	unsigned char *buff = input_ptr;
	unsigned char *output_pt = output_ptr;

    unsigned int r, g, b;
    unsigned int color;
    
	size = image_width * image_height /2;
	for (i = size; i > 0; i--) {
		/* bgr instead rgb ?? */
		Y = buff[0] ;
		U = buff[1] ;
		Y1 = buff[2];
		V = buff[3];
		buff += 4;
        
		r = R_FROMYV(Y,V);
		g = G_FROMYUV(Y,U,V); //b
		b = B_FROMYU(Y,U); //v

        /* 把r,g,b三色构造为rgb565的16位值 */
        r = r >> 3;
        g = g >> 2;
        b = b >> 3;
        color = (r << 11) | (g << 5) | b;
        *output_pt++ = color & 0xff;
        *output_pt++ = (color >> 8) & 0xff;
			
		r = R_FROMYV(Y1,V);
		g = G_FROMYUV(Y1,U,V); //b
		b = B_FROMYU(Y1,U); //v
		
        /* 把r,g,b三色构造为rgb565的16位值 */
        r = r >> 3;
        g = g >> 2;
        b = b >> 3;
        color = (r << 11) | (g << 5) | b;
        *output_pt++ = color & 0xff;
        *output_pt++ = (color >> 8) & 0xff;
	}
	
	return 0;
} 

//translate YUV422Packed to rgb32
static unsigned int yuv422torgb32(unsigned char * input_ptr, unsigned char * output_ptr, 
                                        unsigned int image_width, unsigned int image_height)
{
	unsigned int i, size;
	unsigned char Y, Y1, U, V;
	unsigned char *buff = input_ptr;
	unsigned int *output_pt = (unsigned int *)output_ptr;

    unsigned int r, g, b;
    unsigned int color;

	size = image_width * image_height /2;
	for (i = size; i > 0; i--) {
		/* bgr instead rgb ?? */
		Y = buff[0] ;
		U = buff[1] ;
		Y1 = buff[2];
		V = buff[3];
		buff += 4;

        r = R_FROMYV(Y,V);
		g = G_FROMYUV(Y,U,V); //b
		b = B_FROMYU(Y,U); //v
		/* rgb888 */
		color = (r << 16) | (g << 8) | b;
        *output_pt++ = color;
			
		r = R_FROMYV(Y1,V);
		g = G_FROMYUV(Y1,U,V); //b
		b = B_FROMYU(Y1,U); //v
		color = (r << 16) | (g << 8) | b;
        *output_pt++ = color;
	}
	
	return 0;
} 


static int yuv2rgb_convert(p_video_buffer video_buf_in, p_video_buffer video_buf_out)
{
    p_pixel_datas p_pixel_datas_in  = &video_buf_in->pixel_datas;   
    p_pixel_datas p_pixel_datas_out = &video_buf_out->pixel_datas;   

    p_pixel_datas_out->width  = p_pixel_datas_in->width;
    p_pixel_datas_out->height = p_pixel_datas_in->height;

    initLut(); //from color.c
    
    if (video_buf_out->pixel_format == V4L2_PIX_FMT_RGB565) //rgb565
    {
        p_pixel_datas_out->bpp = 16;
        p_pixel_datas_out->line_bytes  = p_pixel_datas_out->width * p_pixel_datas_out->bpp / 8;
        p_pixel_datas_out->total_bytes = p_pixel_datas_out->line_bytes * p_pixel_datas_out->height;

        if (!p_pixel_datas_out->pixel_datas_addr) //分配空间
        {
            p_pixel_datas_out->pixel_datas_addr = malloc(p_pixel_datas_out->total_bytes);
            if (NULL == p_pixel_datas_out->pixel_datas_addr)
                return -1;
        }
            
        yuv422torgb565(p_pixel_datas_in->pixel_datas_addr, p_pixel_datas_out->pixel_datas_addr, 
                       p_pixel_datas_out->width, p_pixel_datas_out->height);
        return 0;
    }
    else if (video_buf_out->pixel_format == V4L2_PIX_FMT_RGB32) //rgb32
    {
        p_pixel_datas_out->bpp = 32;
        p_pixel_datas_out->line_bytes  = p_pixel_datas_out->width * p_pixel_datas_out->bpp / 8;
        p_pixel_datas_out->total_bytes = p_pixel_datas_out->line_bytes * p_pixel_datas_out->height;

        if (!p_pixel_datas_out->pixel_datas_addr) //分配空间
        {
            p_pixel_datas_out->pixel_datas_addr = malloc(p_pixel_datas_out->total_bytes);
            if (NULL == p_pixel_datas_out->pixel_datas_addr)
                return -1;
        }

        yuv422torgb32(p_pixel_datas_in->pixel_datas_addr, p_pixel_datas_out->pixel_datas_addr, 
                      p_pixel_datas_out->width, p_pixel_datas_out->height);
        return 0;
    }
    
    return -1;

}

static int yuv2rgb_convert_exit(p_video_buffer video_buf_out)
{
    if (video_buf_out->pixel_datas.pixel_datas_addr)
    {
        free(video_buf_out->pixel_datas.pixel_datas_addr);
        video_buf_out->pixel_datas.pixel_datas_addr = NULL;
    }
    
    freeLut(); //from color.c
    
    return 0;
}

static video_convert yuv2rgb_convert_ops = {
    .name           = "yuv2rgb",   
    .judge_support  = is_support_yuv2rgb,
    .convert        = yuv2rgb_convert,  
    .convert_exit   = yuv2rgb_convert_exit,

};

int yuv2rgb_init(void)
{
    return register_video_convert(&yuv2rgb_convert_ops);
}

