
#include <string.h>
#include <config.h>
#include <video_manager.h>
#include <disp_manager.h>

//static p_video_mem video_mem_head = NULL;;
static p_disp_operations disp_ops_head = NULL;
static p_disp_operations default_disp_ops= NULL;

//加入操作函数链表
int register_display_ops(p_disp_operations p_disp_ops)
{
	p_disp_operations p_tmp; 

	if (NULL == disp_ops_head) 
	{
		disp_ops_head      = p_disp_ops;
		p_disp_ops->p_next = NULL;
	}
	else
	{
		p_tmp = disp_ops_head;
		while (p_tmp->p_next)
		{
			p_tmp = p_tmp->p_next;
		}
		p_tmp->p_next      = p_disp_ops;
		p_disp_ops->p_next = NULL;
	}

	return 0;
}

//显示链表中的每个操作函数名字
void show_disp_ops(void)
{
	int i = 0;
	p_disp_operations p_tmp = disp_ops_head;

    printf("display operations lists:\n");

	while (p_tmp)
	{
		printf("%02d %s\n", i++, p_tmp->name);
		p_tmp = p_tmp->p_next;
	}
}

//根据名字从链表获取对应操作函数结构体
p_disp_operations get_disp_ops(char *name)
{
	p_disp_operations p_tmp = disp_ops_head;
	
	while (p_tmp)
	{
		if (strcmp(p_tmp->name, name) == 0)
		{
			return p_tmp;
		}
		p_tmp = p_tmp->p_next;
	}
	return NULL;
}

//根据名字取出指定的"显示模块", 调用它的初始化函数, 并且清屏
void select_and_init_disp_dev(char *name, char *dev_name)
{
	default_disp_ops = get_disp_ops(name);
	if (default_disp_ops)
	{
		default_disp_ops->device_init(dev_name);
		default_disp_ops->clean_screen(0);
	}
}

//返回select_and_init_disp_dev()得到的显示模块
p_disp_operations get_default_disp_dev(void)
{
	return default_disp_ops;
}

//获得所使用的显示设备的分辨率和BPP
int get_disp_resolution(int *x_res, int *y_res, int *bpp)
{
	if (default_disp_ops)
	{
		*x_res = default_disp_ops->x_res;
		*y_res = default_disp_ops->y_res;
		*bpp   = default_disp_ops->bpp;
		return 0;
	}
    
    return -1;
}


int get_video_buf_for_disp(p_video_buffer p_frame_buf)
{
    p_frame_buf->pixel_format = (default_disp_ops->bpp == 16) ? V4L2_PIX_FMT_RGB565 : \
                                (default_disp_ops->bpp == 32) ? V4L2_PIX_FMT_RGB32 : 0;
    p_frame_buf->pixel_datas.width  = default_disp_ops->x_res;
    p_frame_buf->pixel_datas.height = default_disp_ops->y_res;
    p_frame_buf->pixel_datas.bpp    = default_disp_ops->bpp;
    p_frame_buf->pixel_datas.line_bytes    = default_disp_ops->line_width;
    p_frame_buf->pixel_datas.total_bytes   = p_frame_buf->pixel_datas.line_bytes * p_frame_buf->pixel_datas.height;
    p_frame_buf->pixel_datas.pixel_datas_addr = default_disp_ops->dis_mem_addr;
    
    return 0;
}


void flush_pixel_datas_to_dev(p_pixel_datas p_pixel_data)
{
    default_disp_ops->show_page(p_pixel_data);
}

#if 0
//为加快显示速度,事先在缓存中构造好显示的页面的数据
int alloc_video_mem(int num)
{
	int i;

	int x_res = 0;
	int y_res = 0;
	int bpp  = 0;

	int vm_size;
	int line_bytes;

	p_video_mem p_new;

	//确定video_mem的大小
	get_disp_resolution(&x_res, &y_res, &bpp);
    line_bytes = x_res * bpp / 8;
	vm_size = line_bytes * y_res;

	//先把设备本身的framebuffer放入链表;
	//分配一个video_mem结构体, 注意这里没有分配里面的pixel_data.pixel_datas_addr
	//而是让pixel_data.pixel_datas_addr指向显示设备的framebuffer
	p_new = malloc(sizeof(video_mem));
	if (p_new == NULL)
		return -1;

	//指向framebuffer 
	p_new->pixel_data.pixel_datas_addr = default_disp_ops->dis_mem_addr;
	
	p_new->id = 0;
	p_new->dev_framebuffer = 1;        //表示这个video_mem是设备本身的framebuffer, 而不是用作缓存作用的video_mem
	p_new->video_mem_s     = VMS_FREE;
	p_new->pic_s    	   = PS_BLANK;
	p_new->pixel_data.width  = x_res;
	p_new->pixel_data.height = y_res;
	p_new->pixel_data.bpp    = bpp;
	p_new->pixel_data.line_bytes  = line_bytes;
	p_new->pixel_data.total_bytes = vm_size;

	if (num != 0)
	{
		//如果下面要分配用于缓存的video_mem, 
		//把设备本身framebuffer对应的video_mem状态设置为VMS_USED_FOR_CUR,
		//表示这个video_mem不会被作为缓存分配出去
		p_new->video_mem_s = VMS_USED_FOR_CUR;
	}
	
	//放入链表
	p_new->p_next = video_mem_head;
	video_mem_head = p_new;
	
	//分配用于缓存的video_mem
	for (i = 0; i < num; i++)
	{
		//分配video_mem结构体本身和"跟framebuffer同样大小的缓存
		p_new = malloc(sizeof(video_mem) + vm_size);
		if (p_new == NULL)
			return -1;

		//在video_mem结构体里记录上面分配的"跟framebuffer同样大小的缓存"
		p_new->pixel_data.pixel_datas_addr = (unsigned char *)(p_new + 1);

		p_new->id = 0;
		p_new->dev_framebuffer = 0;
		p_new->video_mem_s = VMS_FREE;
		p_new->pic_s       = PS_BLANK;
        p_new->pixel_data.width  = x_res;
        p_new->pixel_data.height = y_res;
        p_new->pixel_data.bpp    = bpp;
        p_new->pixel_data.line_bytes  = line_bytes;
        p_new->pixel_data.total_bytes = vm_size;

		//放入链表 
        p_new->p_next = video_mem_head;
        video_mem_head = p_new;
	}
	
	return 0;
}

//获得一块可操作的video_mem(它用于存储要显示的数据)
p_video_mem get_video_mem(int id, int cur)
{
	p_video_mem p_tmp = video_mem_head;
	
	//1.优先:取出空闲的、ID相同的videomem
	while (p_tmp)
	{
		if ((p_tmp->video_mem_s == VMS_FREE) && (p_tmp->id == id))
		{
			p_tmp->video_mem_s = cur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return p_tmp;
		}
		p_tmp = p_tmp->p_next;
	}

	//2.如果前面不成功, 取出一个空闲的并且里面没有数据(p_video_mem->video_mem_s = PS_BLANK)的video_mem */
	p_tmp = video_mem_head;
	while (p_tmp)
	{
		if ((p_tmp->video_mem_s == VMS_FREE) && (p_tmp->pic_s == PS_BLANK))
		{
			p_tmp->id = id;
			p_tmp->video_mem_s = cur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return p_tmp;
		}
		p_tmp = p_tmp->p_next;
	}	
	
	//3.如果前面不成功: 取出任意一个空闲的video_mem
	p_tmp = video_mem_head;
	while (p_tmp)
	{
		if (p_tmp->video_mem_s == VMS_FREE)
		{
			p_tmp->id = id;
			p_tmp->pic_s = PS_BLANK;
			p_tmp->video_mem_s = cur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return p_tmp;
		}
		p_tmp = p_tmp->p_next;
	}

    //4.如果没有空闲的VideoMem并且bCur为1, 则取出任意一个VideoMem(不管它是否空闲) 
    if (cur)
    {
    	p_tmp = video_mem_head;
    	p_tmp->id = id;
    	p_tmp->pic_s = PS_BLANK;
    	p_tmp->video_mem_s = cur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
    	return p_tmp;
    }
    
	return NULL;
}

//释放get_video_mem()申请的video_mem
void put_video_mem(p_video_mem p_video_m)  /*这里没人调用，有问题*/
{
	p_video_m->video_mem_s = VMS_FREE; //设置VideoMem状态为空闲 
    if (p_video_m->id == -1)
        p_video_m->pic_s = PS_BLANK;   //表示里面的数据没有用了
}

//获得显示设备的显存, 在该显存上操作就可以直接在LCD上显示出来
p_video_mem get_dev_video_mem(void)
{
	p_video_mem p_tmp = video_mem_head;
	
	while (p_tmp)
	{
		if (p_tmp->dev_framebuffer)
			return p_tmp;
		p_tmp = p_tmp->p_next;
	}
	return NULL;
}

//把video_mem中内存全部清为某种颜色
void clear_video_mem(p_video_mem p_video_m, unsigned int color)
{
	int i = 0;
	int red, green, blue;
	unsigned char *p_vm;
	unsigned short *p_vm_16bpp;
	unsigned int *p_vm_32bpp;
	unsigned short color_16bpp; /* 565 */


	p_vm	   = p_video_m->pixel_data.pixel_datas_addr;
	p_vm_16bpp = (unsigned short *)p_vm;
	p_vm_32bpp = (unsigned int *)p_vm;

	switch (p_video_m->pixel_data.bpp)
	{
		case 8:
		{
			memset(p_vm, color, p_video_m->pixel_data.total_bytes);
			break;
		}
		case 16:
		{
			//先根据32位的color构造出16位的color_16bpp 
			red   = (color >> (16+3)) & 0x1f;
			green = (color >> (8+2)) & 0x3f;
			blue  = (color >> 3) & 0x1f;
			color_16bpp = (red << 11) | (green << 5) | blue;
			while (i < p_video_m->pixel_data.total_bytes)
			{
                *p_vm_16bpp	= color_16bpp;
				p_vm_16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while (i < p_video_m->pixel_data.total_bytes)
			{
				*p_vm_32bpp = color;
				p_vm_32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			printf_debug("Can't support %d bpp\n", p_video_m->pixel_data.bpp);
			return;
		}
	}

}

//把video_mem中内存的指定区域全部清为某种颜色
void clear_video_mem_region(p_video_mem p_video_m, p_layout p_lay, unsigned int color)
{
    int i = 0;
    int x, y;
	int red, green, blue;
    int line_bytes_clear;
	unsigned char *p_vm;
	unsigned short *p_vm_16bpp;
	unsigned int *p_vm_32bpp;
	unsigned short color_16bpp; /* 565 */

	p_vm	   = p_video_m->pixel_data.pixel_datas_addr + p_lay->top_left_y * \
                 p_video_m->pixel_data.line_bytes + p_lay->top_left_x * p_video_m->pixel_data.bpp / 8;
	p_vm_16bpp = (unsigned short *)p_vm;
	p_vm_32bpp = (unsigned int *)p_vm;

    line_bytes_clear = (p_lay->bot_right_x - p_lay->top_left_x + 1) * p_video_m->pixel_data.bpp / 8;

	switch (p_video_m->pixel_data.bpp)
	{
		case 8:
		{
            for (y = p_lay->top_left_y; y <= p_lay->bot_right_y; y++)
            {
    			memset(p_vm, color, line_bytes_clear);
                p_vm += p_video_m->pixel_data.line_bytes;
            }
			break;
		}
		case 16:
		{
			//先根据32位的color构造出16位的wColor16bpp
			red   = (color >> (16+3)) & 0x1f;
			green = (color >> (8+2)) & 0x3f;
			blue  = (color >> 3) & 0x1f;
			color_16bpp = (red << 11) | (green << 5) | blue;
            for (y = p_lay->top_left_y; y <= p_lay->bot_right_y; y++)
            {
                i = 0;
                for (x= p_lay->top_left_x; x<= p_lay->bot_right_x; x++)
    			{
    				p_vm_16bpp[i++]	= color_16bpp;
    			}
                p_vm_16bpp = (unsigned short *)((unsigned int)p_vm_16bpp + p_video_m->pixel_data.line_bytes);
            }
			break;
		}
		case 32:
		{
            for (y = p_lay->top_left_y; y <= p_lay->bot_right_y; y++)
            {
                i = 0;
                for (x= p_lay->top_left_x; x<= p_lay->bot_right_x; x++)
    			{
    				p_vm_32bpp[i++]	= color;
    			}
                p_vm_32bpp = (unsigned int *)((unsigned int)p_vm_32bpp + p_video_m->pixel_data.line_bytes);
            }
			break;
		}
		default :
		{
			printf_debug("Can't support %d bpp\n", p_video_m->pixel_data.bpp);
			return;
		}
	}

}
#endif

int display_init(void)
{
	int ret;

    ret = fb_init();

	return ret;
}



