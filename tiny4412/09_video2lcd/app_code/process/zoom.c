
#include <string.h>
#include <stdlib.h>
#include <config.h>
#include <video_manager.h>

//近邻取样插值方法缩放图片
int pic_zoom(p_pixel_datas origin_pic, p_pixel_datas zoom_pic)
{
    unsigned long x, y;
    unsigned long scr_y;
    unsigned char *scr, *dest;
    unsigned long *src_x_table;
    unsigned long dst_width   = zoom_pic->width;
    unsigned long pixel_bytes = origin_pic->bpp / 8;

    printf_debug("src:\n");
    printf_debug("%d x %d, %d bpp, data: 0x%x\n", origin_pic->width, origin_pic->height,
                 origin_pic->bpp, (unsigned int)origin_pic->pixel_datas_addr);

    printf_debug("dest:\n");
    printf_debug("%d x %d, %d bpp, data: 0x%x\n", zoom_pic->width, zoom_pic->height,
                 zoom_pic->bpp, (unsigned int)zoom_pic->pixel_datas_addr);

    if (origin_pic->bpp != zoom_pic->bpp)
        return -1;

    src_x_table = malloc(sizeof(unsigned long) * dst_width);
    if (NULL == src_x_table)
    {
        printf_debug("malloc error!\n");
        return -1;
    }

    for (x = 0; x < dst_width; x++) //生成表 src_x_table
        src_x_table[x] = (x * origin_pic->width / zoom_pic->width);

    for (y = 0; y < zoom_pic->height; y++)
    {
        scr_y = (y * origin_pic->height / zoom_pic->height);

        dest = zoom_pic->pixel_datas_addr + y * zoom_pic->line_bytes;
        scr  = origin_pic->pixel_datas_addr + scr_y * origin_pic->line_bytes;

        for (x = 0; x < dst_width; x++)
        {
            //原图座标: src_x_table[x]，src_y       缩放座标: x, y
            memcpy(dest + x * pixel_bytes, scr + src_x_table[x]*pixel_bytes, pixel_bytes);
        }
    }

    free(src_x_table);

    return 0;
}


