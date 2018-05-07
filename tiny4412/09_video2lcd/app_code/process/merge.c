
#include <string.h>
#include <config.h>
#include <video_manager.h>


//把小图片合并入大图片里
int pic_merge(int x, int y, p_pixel_datas small_pic, p_pixel_datas big_pic)
{
	int i;
	unsigned char *scr;
	unsigned char *dst;
	
	if ((small_pic->width > big_pic->width)   || 
        (small_pic->height > big_pic->height) ||
		(small_pic->bpp != big_pic->bpp))
		return -1;

	scr = small_pic->pixel_datas_addr;
	dst = big_pic->pixel_datas_addr + y * big_pic->line_bytes + x * big_pic->bpp / 8;
	for (i = 0; i < small_pic->height; i++)
	{
		memcpy(dst, scr, small_pic->line_bytes);
		scr += small_pic->line_bytes;
		dst += big_pic->line_bytes;
	}
	return 0;
}

//把新图片的某部分, 合并入老图片的指定区域
int pic_merge_region(int start_x_new_pic, int start_y_new_pic, int start_x_old_pic, int start_y_old_pic, 
                             int width, int height, p_pixel_datas new_pic, p_pixel_datas old_pic)
{
	int i;
	unsigned char *scr;
	unsigned char *dst;
    int line_bytes = width * new_pic->bpp / 8;

    if ((start_x_new_pic < 0 || start_x_new_pic >= new_pic->width)  || \
        (start_y_new_pic < 0 || start_y_new_pic >= new_pic->height) || \
        (start_x_old_pic < 0 || start_x_old_pic >= old_pic->width)  || \
        (start_y_old_pic < 0 || start_y_old_pic >= old_pic->height))
        return -1;
	
	scr = new_pic->pixel_datas_addr + start_y_new_pic * new_pic->line_bytes + start_x_new_pic * new_pic->bpp / 8;
	dst = old_pic->pixel_datas_addr + start_y_old_pic * old_pic->line_bytes + start_x_old_pic * old_pic->bpp / 8;
	for (i = 0; i < height; i++)
	{
		memcpy(dst, scr, line_bytes);
		scr += new_pic->line_bytes;
		dst += old_pic->line_bytes;
	}
	return 0;
}


