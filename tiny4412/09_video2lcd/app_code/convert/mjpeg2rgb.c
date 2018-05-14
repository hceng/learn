
//MJPEG:实质上每一帧数据都是一个完整的JPEG文件
#include <config.h>
#include <convert_manager.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <jpeglib.h>


static int is_support_mjpeg2rgb(int pixel_format_in, int pixel_format_out);
static int mjpeg2rgb_convert(p_video_buffer video_buf_in, p_video_buffer video_buf_out);
static int mjpeg2rgb_convert_exit(p_video_buffer video_buf_out);

//jdatasrc-tj.c
extern void jpeg_mem_src_tj(j_decompress_ptr, unsigned char *, unsigned long);

typedef struct my_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
}my_error_mgr, *p_my_error_mgr;

//参考libjpeg里的bmp.c,自定义的libjpeg库出错处理函数:
//默认的错误处理函数是让程序退出,这里不让程序退出
static void my_error_exit(j_common_ptr cinfo)
{
    static char err_str[JMSG_LENGTH_MAX];
    
	p_my_error_mgr my_err = (p_my_error_mgr)cinfo->err;

    /* Create the message */
    (*cinfo->err->format_message) (cinfo, err_str);
    printf_debug("%s\n", err_str);

	longjmp(my_err->setjmp_buffer, 1);
}

static int is_support_mjpeg2rgb(int pixel_format_in, int pixel_format_out)
{
    if (pixel_format_in != V4L2_PIX_FMT_MJPEG)
        return 0;
    if ((pixel_format_out != V4L2_PIX_FMT_RGB565) && (pixel_format_out != V4L2_PIX_FMT_RGB32))
        return 0;

    return 1;
}

//把已经从JPG文件取出的一行像素数据,转换为能在显示设备上使用的格式
static int covert_one_line(int width, int scr_bpp, int dst_bpp, 
                                  unsigned char *scr_datas, unsigned char *dst_datas)
{
	int i;
	int pos = 0;
	unsigned int red, green, blue, color;
	unsigned short *dst_datas_16bpp = (unsigned short *)dst_datas;
	unsigned int   *dst_datas_32bpp = (unsigned int *)dst_datas;
	
	if (scr_bpp != 24)
		return -1;
	
	if (dst_bpp == 24)
		memcpy(dst_datas, scr_datas, width*3); //len=width*(8+8+8)/8=width*3
	else
	{
		for (i = 0; i < width; i++)
		{
			red   = scr_datas[pos++];
			green = scr_datas[pos++];
			blue  = scr_datas[pos++];
			if (dst_bpp == 32)
			{
				color = (red << 16) | (green << 8) | blue;
				*dst_datas_32bpp = color;
				dst_datas_32bpp++;
			}
			else if (dst_bpp == 16)
			{
				/* 565 */
				red   = red >> 3;
				green = green >> 2;
				blue  = blue>> 3;
				color = (red << 11) | (green << 5) | (blue);
				*dst_datas_16bpp = color;
				dst_datas_16bpp++;
			}
		}
	}
	return 0;
}


static int mjpeg2rgb_convert(p_video_buffer video_buf_in, p_video_buffer video_buf_out)
{
    
    int row_stride; //一行的数据
    my_error_mgr jerr;
    unsigned char *p_dest; 
    unsigned char *line_buf = NULL;
    p_pixel_datas p_pixel_data = &video_buf_out->pixel_datas;

    //1.分配jpeg对象结构体空间,并初始化(使用自定义的出错处理方式)
    struct jpeg_decompress_struct cinfo;
    
    cinfo.err = jpeg_std_error(&jerr.pub); //绑定jerr错误结构体至jpeg对象结构体
    jerr.pub.error_exit = my_error_exit; //设置为自己定义的出错处理函数

    if(setjmp(jerr.setjmp_buffer))
	{
		//果程序能运行到这里, 表示JPEG解码出错
		printf_debug("Decoding error.\n");
        jpeg_destroy_decompress(&cinfo);
        if (line_buf)            
            free(line_buf);
        if (p_pixel_data->pixel_datas_addr)            
            free(p_pixel_data->pixel_datas_addr);

		return -1;
	}
    jpeg_create_decompress(&cinfo); //初始化cinfo结构体
	
    //2.指定解压数据源据(内存中的数据)
    jpeg_mem_src_tj(&cinfo, video_buf_in->pixel_datas.pixel_datas_addr, video_buf_in->pixel_datas.total_bytes);

    //3.获取解压文件信息
    jpeg_read_header(&cinfo, TRUE); //将图像的缺省信息填充到cinfo结构中以便程序使用

    //4.为解压设定参数,包括图像大小和颜色空间
    cinfo.scale_num = cinfo.scale_denom = 1; //1:1

    //5.开始解压缩
    jpeg_start_decompress(&cinfo);

    //6.取数据
    row_stride = cinfo.output_width * cinfo.output_components;
    line_buf = malloc(row_stride);
    if (NULL == line_buf)
        return -1;

    p_pixel_data->width       = cinfo.output_width;
    p_pixel_data->height      = cinfo.output_height;
    p_pixel_data->line_bytes  =  p_pixel_data->width *  p_pixel_data->bpp /8;
    p_pixel_data->total_bytes = p_pixel_data->line_bytes * p_pixel_data->height;
    if (NULL == p_pixel_data->pixel_datas_addr)
	{
	    p_pixel_data->pixel_datas_addr = malloc(p_pixel_data->total_bytes);
        if (NULL == p_pixel_data->pixel_datas_addr)
            return -1;
	}
	p_dest = p_pixel_data->pixel_datas_addr;
	
    //循环调用jpeg_read_scanlines来一行一行地获得解压的数据
    while (cinfo.output_scanline < cinfo.output_height) 
	{
        //得到一行数据,里面的颜色格式为0xRR, 0xGG, 0xBB
		(void) jpeg_read_scanlines(&cinfo, &line_buf, 1);

		//转到p_pixel_data去
		covert_one_line(p_pixel_data->width, 24, p_pixel_data->bpp, line_buf, p_dest);
		p_dest += p_pixel_data->line_bytes;
	}

    //7.解压完毕,释放资源
    free(line_buf);
    //jpeg_finish_decompress(&cinfo); //实测加上很大概率启动报错
	//http://blog.sina.com.cn/s/blog_78ea87380101g42c.html
    jpeg_destroy_decompress(&cinfo);
	
	return 0;
}

static int mjpeg2rgb_convert_exit(p_video_buffer video_buf_out)
{
    if (video_buf_out->pixel_datas.pixel_datas_addr)
    {
        free(video_buf_out->pixel_datas.pixel_datas_addr);
        video_buf_out->pixel_datas.pixel_datas_addr = NULL;
    }
    
    return 0;
}

static video_convert mjpeg2rgb_convert_ops = {
    .name           = "mjpeg2rgb",   
    .judge_support  = is_support_mjpeg2rgb,
    .convert        = mjpeg2rgb_convert,  
    .convert_exit   = mjpeg2rgb_convert_exit,
};

int mjpeg2rgb_init(void)
{
    return register_video_convert(&mjpeg2rgb_convert_ops);
}

