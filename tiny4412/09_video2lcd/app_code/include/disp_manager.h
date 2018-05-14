
#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

#include <video_manager.h>

#if 0
//表示显示的区域
typedef struct layout {
	int top_left_x;  //区域左上角x坐标
	int top_left_y;  //区域左上角y坐标
	int bot_right_x; //区域右下角x坐标
	int bot_right_y; //区域右下角y坐标
	char *icon_name; //如果是图标,则为图片名
}layout, *p_layout;

//video_mem的状态
typedef enum {
	VMS_FREE = 0,         //空闲
	VMS_USED_FOR_PREPARE, //用于预先准备显示内容
	VMS_USED_FOR_CUR,	  //用于当前线程
}video_mem_state;

//video_mem中内存里图片的状态
typedef enum {
	PS_BLANK = 0,  //空白
	PS_GENERATING, //正在生成
	PS_GENERATED,  //已经生成
}pic_state;

typedef struct video_mem {
	int id;                      //ID值,用于标识不同的页面 
	int dev_framebuffer;         //1: 这个VideoMem是显示设备的显存; 0: 只是一个普通缓存 
	video_mem_state video_mem_s; //这个VideoMem的状态 
	pic_state pic_s;             //video_mem中内存里图片的状态
	pixel_datas pixel_data;      //内存: 用来存储图像
	struct video_mem *p_next;    //链表 
}video_mem, *p_video_mem;
#endif


typedef struct disp_operations {
	char *name;                   //显示模块的名字
	int x_res;                    //X分辨率 
	int y_res;                    //Y分辨率
	int bpp;                      //一个像素用多少位来表示 
	int line_width;               //一行数据占据多少字节
	unsigned char *dis_mem_addr;  //显存地址 
	int (*device_init)(char *name);     //设备初始化函数 
	int (*show_pixel)(int pen_x, int pen_y, unsigned int color); //把指定座标的像素设为某颜色
	int (*clean_screen)(unsigned int back_color);                //清屏为某颜色
	int (*show_page)(p_pixel_datas p_pixel_data);                //显示一页,数据源自p_video_mem
	struct disp_operations *p_next;                             //链表
}disp_operations, *p_disp_operations;

//disp_manager
int register_display_ops(p_disp_operations p_disp_ops);
void show_disp_ops(void);
p_disp_operations get_disp_ops(char *name);
void select_and_init_disp_dev(char *name, char *dev_name);
p_disp_operations get_default_disp_dev(void);
int get_disp_resolution(int *x_res, int *y_res, int *bpp);
int get_video_buf_for_disp(p_video_buffer p_frame_buf);
void flush_pixel_datas_to_dev(p_pixel_datas p_pixel_data);
int display_init(void);
int fb_init(void);

#if 0
int alloc_video_mem(int num);
p_video_mem get_video_mem(int id, int cur);
void put_video_mem(p_video_mem p_video_m);
p_video_mem get_dev_video_mem(void);
void clear_video_mem(p_video_mem p_video_m, unsigned int color);
void clear_video_mem_region(p_video_mem p_video_m, p_layout p_lay, unsigned int color);
#endif

#endif


