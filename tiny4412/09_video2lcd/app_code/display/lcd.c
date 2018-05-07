
#include <config.h>
#include <disp_manager.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>

static int fd;
static struct fb_var_screeninfo fb_var;
static struct fb_fix_screeninfo fb_fix;			
static unsigned char *fb_mem;
static unsigned int screen_size;
static unsigned int line_width;
static unsigned int pixel_width;

//fb
static disp_operations disp_ops;
static int fb_device_init(char *dev_name);     
static int fb_show_pixel(int pen_x, int pen_y, unsigned int color); 
static int fb_clean_screen(unsigned int back_color);              
static int fb_show_page(p_pixel_datas p_pixel_data);          

static int fb_device_init(char *dev_name)
{
	int ret;
	
	if (NULL == dev_name)
        dev_name = FB_DEVICE_NAME;
	
	fd = open(dev_name, O_RDWR);
	if (0 > fd)
		printf_debug("Can't open %s\n", dev_name);

	ret = ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
	if (ret < 0)
	{
		printf_debug("Can't get fb's var\n");
		return -1;
	}

	ret = ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix);
	if (ret < 0)
	{
		printf_debug("Can't get fb's fix\n");
		return -1;
	}
	
	screen_size = fb_var.xres * fb_var.yres * fb_var.bits_per_pixel / 8;
	fb_mem = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (0 > fb_mem)	
	{
		printf_debug("Can't mmap\n");
		return -1;
	}

	disp_ops.x_res        = fb_var.xres;
	disp_ops.y_res        = fb_var.yres;
	disp_ops.bpp          = fb_var.bits_per_pixel;
	disp_ops.line_width   = fb_var.xres * disp_ops.bpp / 8;
	disp_ops.dis_mem_addr = fb_mem;

	line_width  = fb_var.xres * fb_var.bits_per_pixel / 8;
	pixel_width = fb_var.bits_per_pixel / 8;
	
	return 0;
}

static int fb_show_pixel(int pen_x, int pen_y, unsigned int color)
{
	int red, green, blue;
	unsigned char *p_fb;
	unsigned short *p_fb_16bpp;
	unsigned int *p_fb_32bpp;
	unsigned short color_16bpp; /* 565 */

	if ((pen_x >= fb_var.xres) || (pen_y >= fb_var.yres))
	{
		printf_debug("Out of region\n");
		return -1;
	}

	p_fb       = fb_mem + line_width * pen_y + pixel_width * pen_x;
	p_fb_16bpp = (unsigned short *)p_fb;
	p_fb_32bpp = (unsigned int *)p_fb;
	
	switch (fb_var.bits_per_pixel)
	{
		case 8:
		{
			*p_fb = (unsigned char)color;
			break;
		}
		case 16:
		{
			//从back_color中取出红绿蓝三原色,构造为16Bpp的颜色
			red   = (color >> (16+3)) & 0x1f;
			green = (color >> (8+2)) & 0x3f;
			blue  = (color >> 3) & 0x1f;
			color_16bpp = (red << 11) | (green << 5) | blue;
			*p_fb_16bpp	= color_16bpp;
			break;
		}
		case 32:
		{
			*p_fb_32bpp = color;
			break;
		}
		default :
		{
			printf_debug("Can't support %d bpp\n", fb_var.bits_per_pixel);
			return -1;
		}
	}

	return 0;

}


static int fb_clean_screen(unsigned int back_color)   
{
    int i = 0;
	int red, green, blue;
	unsigned char *p_fb;
	unsigned short *p_fb_16bpp;
	unsigned int *p_fb_32bpp;
	unsigned short color_16bpp; /* 565 */


	p_fb      = fb_mem;
	p_fb_16bpp  = (unsigned short *)p_fb;
	p_fb_32bpp = (unsigned int *)p_fb;

	switch (fb_var.bits_per_pixel)
	{
		case 8:
		{
			memset(fb_mem, back_color, screen_size);
			break;
		}
		case 16:
		{
			/* 从back_color中取出红绿蓝三原色,
			 * 构造为16Bpp的颜色
			 */
			red   = (back_color >> (16+3)) & 0x1f;
			green = (back_color >> (8+2)) & 0x3f;
			blue  = (back_color >> 3) & 0x1f;
			color_16bpp = (red << 11) | (green << 5) | blue;
			while (i < screen_size)
			{
				*p_fb_16bpp	= color_16bpp;
				p_fb_16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while (i < screen_size)
			{
				*p_fb_32bpp	= back_color;
				p_fb_32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			printf_debug("Can't support %d bpp\n", fb_var.bits_per_pixel);
			return -1;
		}
	}

	return 0;

}

static int fb_show_page(p_pixel_datas p_pixel_data)  
{
    if (disp_ops.dis_mem_addr != p_pixel_data->pixel_datas_addr)
    	memcpy(disp_ops.dis_mem_addr, p_pixel_data->pixel_datas_addr, p_pixel_data->total_bytes);
    
	return 0;
}

static disp_operations disp_ops = {
	.name         = "lcd",
	.device_init  = fb_device_init,
	.show_pixel   = fb_show_pixel,
	.clean_screen = fb_clean_screen,
	.show_page    = fb_show_page,
};

int fb_init(void)
{
	return register_display_ops(&disp_ops);
}




