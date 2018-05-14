#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>

#include <config.h>
#include <process.h>
#include <disp_manager.h>
#include <video_manager.h>
#include <convert_manager.h>


static void print_help(void)
{
    printf("Usage: video2lcd [options]... [FILE]...\n");
    printf("The LCD displays the image captured by the camera.\n");
    printf("Options:\n");
    printf("\t" "-v" "\t\tSelect the camera device, default: /dev/video0\n");
    printf("\t" "-d" "\t\tSelect the lcd display device, default: /dev/fb0\n");
    printf("\t" "-h" "\t\tDisplay this information.\n");
}


static void stop_app(int signo)
{
    printf("\nexit.\n");

    _exit(0);
}


int main(int argc, char **argv)
{
    int i, ret;
    float k;
    video_device video_dev;
    p_video_convert video_conv;
    int pixel_formt_of_video, pixel_formt_of_disp;

    int top_left_x, top_left_y;
    int lcd_width, lcd_height, lcd_bpp;

    p_video_buffer video_buf_cur;
    video_buffer video_buf, convert_buf, zoom_buf, frame_buf;

    char *get_argv[2] = {};

    signal(SIGINT, stop_app);

    //0.传入参数判断
    for(i = 1; i < argc; i++)
    {
        if (!strcmp("-v", argv[i]))
        {
            if(NULL == argv[i + 1])
            {
                print_help();
                return -1;
            }
            else
                get_argv[0] = argv[i + 1];
        }
        else if (!strcmp("-d", argv[i]))
        {
            if(NULL == argv[i + 1])
            {
                print_help();
                return -1;
            }
            else
                get_argv[1] = argv[i + 1];
        }
        else if (!strcmp("-h", argv[i]))
        {
            print_help();
            return 0;
        }
    }

    //1.初始化显示设备并获取显示设备参数
    display_init(); //注册所有显示设备(fb和crt)

    if (get_argv[1] == NULL) //选择和初始化指定的显示设备
        select_and_init_disp_dev("lcd", "/dev/fb0"); //default:lcd的/dev/fb0
    else
        select_and_init_disp_dev("lcd", get_argv[1]);

    get_disp_resolution(&lcd_width, &lcd_height, &lcd_bpp); //获取设备的分辨率和支持的bpp
    get_video_buf_for_disp(&frame_buf); //得到显存的各种信息(分辨率、bpp、大小、地址等)

    pixel_formt_of_disp = frame_buf.pixel_format;

    //2.初始化采集设备
    video_init(); //注册所有图像采集设备(v4l2协议)

    ret = video_device_init(get_argv[0], &video_dev); //初始化指定的/dev/video0
    if (ret)
    {
        printf_debug("video_device_init for %s error!\n", get_argv[0]);
        return -1;
    }
    pixel_formt_of_video = video_dev.p_video_fops->get_format(&video_dev); //获取视频格式

    //3.转换初始化
    video_convert_init(); //注册所有支持的转换方式(yuv、mjpeg、rgb)
    //传入采集设备格式和显示设备支持格式,在链表里依次判断是否支持该格式转换
    video_conv = get_video_convert_format(pixel_formt_of_video, pixel_formt_of_disp);
    if (NULL == video_conv)
    {
        printf_debug("Can not support this format convert\n");
        return -1;
    }


    //4.启动摄像头设备
    ret = video_dev.p_video_fops->start_device(&video_dev);
    if (ret)
    {
        printf_debug("start_device for %s error!\n", get_argv[0]);
        return -1;
    }

    memset(&video_buf, 0, sizeof(video_buf));
    memset(&convert_buf, 0, sizeof(convert_buf));
    convert_buf.pixel_format    = pixel_formt_of_disp;
    convert_buf.pixel_datas.bpp = lcd_bpp;


    memset(&zoom_buf, 0, sizeof(zoom_buf));

    while (1)
    {
        //5.读入摄像头数据
        ret = video_dev.p_video_fops->get_frame(&video_dev, &video_buf);
        if (ret)
        {
            printf_debug("get_frame for %s error!\n", get_argv[0]);
            return -1;
        }
        video_buf_cur = &video_buf;

        if (pixel_formt_of_video != pixel_formt_of_disp) //采集的图像格式和显示的图像格式不一致
        {
            //6.格式转换
            ret = video_conv->convert(&video_buf, &convert_buf);
            if (ret)
            {
                printf_debug("convert for %s error!\n", get_argv[0]);
                return -1;
            }
            video_buf_cur = &convert_buf;
        }
        //现在video_buf_cur就是最后的图像数据


        //7.如果图像分辨率大于LCD, 缩放
        if ((video_buf_cur->pixel_datas.width > lcd_width) || (video_buf_cur->pixel_datas.height > lcd_height))
        {
            //确定缩放后的分辨率
            //把图片按比例缩放到video_mem上, 居中显示
            //1. 先算出缩放后的大小
            k = (float)video_buf_cur->pixel_datas.height / video_buf_cur->pixel_datas.width; //长宽比例
            zoom_buf.pixel_datas.width  = lcd_width;
            zoom_buf.pixel_datas.height = lcd_width * k;
            if ( zoom_buf.pixel_datas.height > lcd_height)
            {
                zoom_buf.pixel_datas.width  = lcd_height / k;
                zoom_buf.pixel_datas.height = lcd_height;
            }

            zoom_buf.pixel_datas.bpp         = lcd_bpp;
            zoom_buf.pixel_datas.line_bytes  = zoom_buf.pixel_datas.width * zoom_buf.pixel_datas.bpp / 8;
            zoom_buf.pixel_datas.total_bytes = zoom_buf.pixel_datas.line_bytes * zoom_buf.pixel_datas.height;

            if (!zoom_buf.pixel_datas.pixel_datas_addr)
            {
                zoom_buf.pixel_datas.pixel_datas_addr = malloc(zoom_buf.pixel_datas.total_bytes);
                if (NULL == zoom_buf.pixel_datas.pixel_datas_addr)
                    return -1;
            }

            pic_zoom(&video_buf_cur->pixel_datas, &zoom_buf.pixel_datas);
            video_buf_cur = &zoom_buf;
        }

        //合并进framebuffer
        //接着算出居中显示时左上角坐标
        top_left_x = (lcd_width - video_buf_cur->pixel_datas.width) / 2;
        top_left_y = (lcd_height - video_buf_cur->pixel_datas.height) / 2;

        pic_merge(top_left_x, top_left_y, &video_buf_cur->pixel_datas, &frame_buf.pixel_datas);

        flush_pixel_datas_to_dev(&frame_buf.pixel_datas);

        ret = video_dev.p_video_fops->put_frame(&video_dev, &video_buf);
        if (ret)
        {
            printf_debug("put_frame for %s error!\n", get_argv[0]);
            return -1;
        }

        //把framebuffer的数据刷到LCD上, 显示
    }

    return 0;
}


