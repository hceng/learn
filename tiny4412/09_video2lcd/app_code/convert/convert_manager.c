
#include <convert_manager.h>
#include <string.h>


static p_video_convert p_video_convert_ops_head = NULL;

//注册到链表
int register_video_convert(p_video_convert p_video_conv)
{
	p_video_convert p_tmp; 

	if (NULL == p_video_convert_ops_head) 
	{
		p_video_convert_ops_head    = p_video_conv;
		p_video_conv->p_next = NULL;
	}
	else
	{
		p_tmp = p_video_convert_ops_head;
		while (p_tmp->p_next)
		{
			p_tmp = p_tmp->p_next;
		}
		p_tmp->p_next       = p_video_conv;
		p_video_conv->p_next = NULL;
	}

	return 0;
}

//显示链表中的每个操作函数名字
void show_video_convert_ops(void)
{
	int i = 0;
	p_video_convert p_tmp = p_video_convert_ops_head;

    printf("video convert operations lists:\n");

	while (p_tmp)
	{
		printf("%02d %s\n", i++, p_tmp->name);
		p_tmp = p_tmp->p_next;
	}
}

//根据名字从链表获取对应操作函数结构体
p_video_convert get_video_convert_ops(char *name)
{
	p_video_convert p_tmp = p_video_convert_ops_head;
	
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

p_video_convert get_video_convert_format(int pixel_format_in, int pixel_format_out)
{
	p_video_convert p_tmp = p_video_convert_ops_head;
	
	while (p_tmp)
	{
        if (p_tmp->judge_support(pixel_format_in, pixel_format_out))
        {
            return p_tmp;
        }
		p_tmp = p_tmp->p_next;
	}
	return NULL;
}

int video_convert_init(void)
{
	int ret;

    ret = yuv2rgb_init();
    ret |= mjpeg2rgb_init();
    ret |= rgb2rgb_init();

	return ret;
}

