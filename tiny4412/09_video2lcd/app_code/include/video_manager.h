
#ifndef _VIDEO_MANAGER_H
#define _VIDEO_MANAGER_H

#define VIDEO_BUFFER_NUM  2


//图片像素的数据
typedef struct pixel_datas {
	int width;                       //宽度: 一行有多少个像素
	int height;                      //高度: 一列有多少个像素
	int bpp;                         //一个像素用多少位来表示
	int line_bytes;                  //一行数据有多少字节
	int total_bytes;                 //所有字节数
	unsigned char *pixel_datas_addr; //像素数据存储的地址
}pixel_datas, *p_pixel_datas;


//摄像头的数据
typedef struct video_buffer {
    pixel_datas pixel_datas;   //图片像素的数据
    int pixel_format;          //像素的格式
}video_buffer, *p_video_buffer;


typedef struct video_operations video_operations, *p_video_operations;
//摄像头设备
typedef struct video_device {
    int fd;            //文件句柄
    int pixel_format;  //像素格式
    int width, height; //分辨率:宽*高

    int buf_count;     //buf数量
    int buf_maxlen;    //每个buf最大长度
    int buf_cur_index; //当前buf索引
    unsigned char *video_buf[VIDEO_BUFFER_NUM]; //每个video buf的地址

    //操作函数
    p_video_operations p_video_fops;
    
}video_device, *p_video_device;

//摄像头设备的操作函数
typedef struct video_operations {
    char *name;

    int (*init_device)(char *dev_name, p_video_device p_video_dev);
    int (*exit_device)(p_video_device p_video_dev);
    
    int (*get_format)(p_video_device p_video_dev);
    
    int (*get_frame)(p_video_device p_video_dev, p_video_buffer p_video_buf); 
    int (*put_frame)(p_video_device p_video_dev, p_video_buffer p_video_buf); 

    int (*start_device)(p_video_device p_video_dev);
    int (*stop_device)(p_video_device p_video_dev);

    struct video_operations *p_next;
}video_operations, *p_video_operations;


//video_manager
int register_video_ops(p_video_operations p_video_ops);
void show_video_ops(void);
p_video_operations get_video_ops(char *name);
int video_device_init(char *dev_name, p_video_device p_video_dev);
int video_init(void);
int v4l2_init(void);


#endif

