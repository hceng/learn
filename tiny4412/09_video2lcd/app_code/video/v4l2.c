
#include <config.h>
#include <video_manager.h>
#include <disp_manager.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>


//v4l2
static int v4l2_init_device(char *dev_name, p_video_device p_video_dev);
static int v4l2_exit_device(p_video_device p_video_dev);
static int v4l2_get_format(p_video_device p_video_dev);
static int v4l2_get_frame_readwrite(p_video_device p_video_dev, p_video_buffer p_video_buf);
static int v4l2_put_frame_readwrite(p_video_device p_video_dev, p_video_buffer p_video_buf);
static int v4l2_get_frame_readwrite(p_video_device p_video_dev, p_video_buffer p_video_buf);
static int v4l2_put_frame_readwrite(p_video_device p_video_dev, p_video_buffer p_video_buf);
static int v4l2_start_device(p_video_device p_video_dev);
static int v4l2_stop_device(p_video_device p_video_dev);




static int v4l2_format_support_list[] = {V4L2_PIX_FMT_YUYV, V4L2_PIX_FMT_MJPEG, V4L2_PIX_FMT_RGB565};

static video_operations v4l2_video_ops;


//实现构造函数
static int v4l2_init_device(char *dev_name, p_video_device p_video_dev)
{
    int fd, ret, i;
    struct v4l2_capability v4l2_cap;
    struct v4l2_fmtdesc v4l2_format_des;
    struct v4l2_format  v4l2_fmt;
    struct v4l2_requestbuffers v4l2_req_buf;
    struct v4l2_buffer v4l2_buf;

    int lcd_width, lcd_heigt, lcd_bpp;


    if (NULL == dev_name)
        dev_name = VIDEO_DEVICE_NAME;

    fd = open(dev_name, O_RDWR);
    if (fd < 0)
    {
        printf_debug("Can't open device: %s\n", dev_name);
        return -1;
    }
    p_video_dev->fd = fd;

    //1.VIDIOC_QUERYCAP:获取设备信息(是否为摄像头、名字、版本等)
    memset(&v4l2_cap, 0, sizeof(struct v4l2_capability));
    ret = ioctl(fd, VIDIOC_QUERYCAP, &v4l2_cap);
    if (0 != ret)
    {
        printf_debug("Can't VIDIOC_QUERYCAP device\n");
        goto err_exit;
    }

    //是否为视频采集设备
    if (!(v4l2_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        printf_debug("%s is not a video capture device\n", dev_name);
        goto err_exit;
    }

    //查询支持何种接口(streaming/read)
    if (v4l2_cap.capabilities & V4L2_CAP_STREAMING)
        printf_debug("%s supports streaming i/o\n", dev_name);
    if (v4l2_cap.capabilities & V4L2_CAP_READWRITE)
        printf_debug("%s supports read i/o\n", dev_name);


    //2.VIDIOC_ENUM_FMT:查询支持哪些种格式
    memset(&v4l2_format_des, 0, sizeof(struct v4l2_fmtdesc));
	v4l2_format_des.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //不可省
    while (0 == ioctl(fd, VIDIOC_ENUM_FMT, &v4l2_format_des))
    {

        if (v4l2_format_des.type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
            break;

        for (i = 0; i < sizeof(v4l2_format_support_list) / sizeof(v4l2_format_support_list[0]); i++)
        {
            if (v4l2_format_support_list[i] == v4l2_format_des.pixelformat)
            {
                p_video_dev->pixel_format = v4l2_format_des.pixelformat;
                break;
            }

        }
        v4l2_format_des.index++;
    }

    if (!p_video_dev->pixel_format)
    {
        printf_debug("Can't VIDIOC_ENUM_FMT device\n");
        printf_debug("Can't support the format of this device\n");
        goto err_exit;
    }

    //3.VIDIOC_S_FMT:设置设备使用何种格式
    get_disp_resolution(&lcd_width, &lcd_heigt, &lcd_bpp); //获取LCD信息
    memset(&v4l2_fmt, 0, sizeof(struct v4l2_format));
    v4l2_fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_fmt.fmt.pix.pixelformat = p_video_dev->pixel_format;
    //v4l2_fmt.fmt.pix.pixelformat = p_video_dev->pixel_format = V4L2_PIX_FMT_MJPEG;
    v4l2_fmt.fmt.pix.width       = lcd_width;
    v4l2_fmt.fmt.pix.height      = lcd_heigt;
    v4l2_fmt.fmt.pix.field       = V4L2_FIELD_ANY;

    //如果驱动无法支持某些参数(比如分辨率),它会调整这些参数,并返回
    ret = ioctl(fd, VIDIOC_S_FMT, &v4l2_fmt);
    if (0 != ret)
    {
        printf_debug("Can't VIDIOC_S_FMT device\n");
        goto err_exit;
    }
    p_video_dev->width  = v4l2_fmt.fmt.pix.width;
    p_video_dev->height = v4l2_fmt.fmt.pix.height;


    //4.VIDIOC_REQBUFS:申请buf
    memset(&v4l2_req_buf, 0, sizeof(struct v4l2_requestbuffers));
    v4l2_req_buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_req_buf.count  = VIDEO_BUFFER_NUM;
    v4l2_req_buf.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(fd, VIDIOC_REQBUFS, &v4l2_req_buf);
    if (ret < 0)
    {
        printf_debug("Can't VIDIOC_REQBUFS device\n");
        goto err_exit;
    }

    //记录实际得到的buf个数
    p_video_dev->buf_count = v4l2_req_buf.count;
    printf_debug("Request buffers count is: %d\n", p_video_dev->buf_count);

    //5.根据接口类型进行对应操作(streaming接口需要映射)
    if (v4l2_cap.capabilities & V4L2_CAP_STREAMING)
    {

        for (i = 0; i < p_video_dev->buf_count; i++)
        {
            //6.1查询分配的buf(获得每个buf大小、偏移)
            memset(&v4l2_buf, 0, sizeof(struct v4l2_buffer));
            v4l2_buf.index  = i;
            v4l2_buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            v4l2_buf.memory = V4L2_MEMORY_MMAP;
            ret = ioctl(fd, VIDIOC_QUERYBUF, &v4l2_buf);
            if (0 != ret)
            {
                printf_debug("Can't VIDIOC_QUERYBUF device\n");
                goto err_exit;
            }

            //6.2映射buf到用户空间(将用户空间buf和内核空间buf 进行绑定)
            p_video_dev->buf_maxlen   = v4l2_buf.length;
            p_video_dev->video_buf[i] = mmap(0, v4l2_buf.length, PROT_READ, MAP_SHARED, fd, v4l2_buf.m.offset);
            if (p_video_dev->video_buf[i] == MAP_FAILED)
            {
                printf_debug("Can't map buffer\n");
                goto err_exit;
            }
        }

        //6.3将映射的buf放入驱动的buf队列
        for (i = 0; i < p_video_dev->buf_count; i++)
        {
            memset(&v4l2_buf, 0, sizeof(struct v4l2_buffer));
            v4l2_buf.index  = i;
            v4l2_buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            v4l2_buf.memory = V4L2_MEMORY_MMAP;
            ret = ioctl(fd, VIDIOC_QBUF, &v4l2_buf);
            if (0 != ret)
            {
                printf_debug("Can't VIDIOC_QBUF device\n");
                goto err_exit;
            }
        }

    }
    else if (v4l2_cap.capabilities & V4L2_CAP_READWRITE)
    {
        //重新设置为readwrite模式的操作函数
        v4l2_video_ops.get_frame = v4l2_get_frame_readwrite;
        v4l2_video_ops.put_frame = v4l2_put_frame_readwrite;

        //read(fd, buf, size)
        p_video_dev->buf_count    = 1;
        //在这个程序所能支持的格式里,一个像素最多只需要4字节
        p_video_dev->buf_maxlen   = p_video_dev->width * p_video_dev->height * 4;
        p_video_dev->video_buf[0] = malloc(p_video_dev->buf_maxlen);
    }

    p_video_dev->p_video_fops = &v4l2_video_ops; //绑定操作函数

    return 0;

err_exit:

    close(fd);
    p_video_dev->fd = 0;
    return -1;

}

static int v4l2_exit_device(p_video_device p_video_dev)
{
    int i;

    for (i = 0; i < p_video_dev->buf_count; i++)
    {
        if (p_video_dev->video_buf[i])
        {
            munmap(p_video_dev->video_buf[i], p_video_dev->buf_maxlen);
            p_video_dev->video_buf[i] = NULL;
        }
    }

    close(p_video_dev->fd);

    return 0;
}


static int v4l2_get_format(p_video_device p_video_dev)
{
    return p_video_dev->pixel_format;
}


static int v4l2_get_frame_streaming(p_video_device p_video_dev, p_video_buffer p_video_buf)
{
    //1.poll查询是否有数据
    int ret;
    struct pollfd poll_fd[1];
    struct v4l2_buffer v4l2_buf;

    poll_fd[0].fd     = p_video_dev->fd;
    poll_fd[0].events = POLLIN; //数据可读

    ret = poll(poll_fd, 1, -1); //poll_fd;项数;超时时间
    if (ret <= 0)
    {
        printf_debug("Can't poll\n");
        return -1;
    }

    //2.VIDIOC_DQBUF从队列取出数据
    memset(&v4l2_buf, 0, sizeof(struct v4l2_buffer));
    v4l2_buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(p_video_dev->fd, VIDIOC_DQBUF, &v4l2_buf);
    if (ret < 0)
    {
        printf_debug("Can't VIDIOC_DQBUF device\n");
        return -1;
    }
    p_video_dev->buf_cur_index = v4l2_buf.index; //方便放入时找到对应节点buf

    p_video_buf->pixel_format       = p_video_dev->pixel_format;
    p_video_buf->pixel_datas.width  = p_video_dev->width;
    p_video_buf->pixel_datas.height = p_video_dev->height;
    p_video_buf->pixel_datas.bpp    = (p_video_dev->pixel_format == V4L2_PIX_FMT_YUYV) ? 16 : \
                                      (p_video_dev->pixel_format == V4L2_PIX_FMT_MJPEG) ? 0 : \
                                      (p_video_dev->pixel_format == V4L2_PIX_FMT_RGB565) ? 16 : 0;
    p_video_buf->pixel_datas.line_bytes       = p_video_dev->width * p_video_buf->pixel_datas.bpp / 8;
    p_video_buf->pixel_datas.total_bytes      = v4l2_buf.bytesused; //数据有多少字节
    p_video_buf->pixel_datas.pixel_datas_addr = p_video_dev->video_buf[v4l2_buf.index];

    return 0;
}

static int v4l2_put_frame_streaming(p_video_device p_video_dev, p_video_buffer p_video_buf)
{

    //1.VIDIOC_QBUF放入队列
    int ret;
    struct v4l2_buffer v4l2_buf;

    memset(&v4l2_buf, 0, sizeof(struct v4l2_buffer));
    v4l2_buf.index  = p_video_dev->buf_cur_index;
    v4l2_buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_buf.memory = V4L2_MEMORY_MMAP;

    ret = ioctl(p_video_dev->fd, VIDIOC_QBUF, &v4l2_buf);
    if (ret < 0)
    {
        printf_debug("Can't VIDIOC_QBUF device\n");
        return -1;
    }

    return 0;
}

static int v4l2_get_frame_readwrite(p_video_device p_video_dev, p_video_buffer p_video_buf)
{
    int ret;

    ret = read(p_video_dev->fd, p_video_dev->video_buf[0], p_video_dev->buf_maxlen);
    if (ret <= 0)
    {
        printf_debug("Can't read\n");
        return -1;
    }

    p_video_buf->pixel_format       = p_video_dev->pixel_format;
    p_video_buf->pixel_datas.width  = p_video_dev->width;
    p_video_buf->pixel_datas.height = p_video_dev->height;
    p_video_buf->pixel_datas.bpp    = (p_video_dev->pixel_format == V4L2_PIX_FMT_YUYV) ? 16 : \
                                      (p_video_dev->pixel_format == V4L2_PIX_FMT_MJPEG) ? 0 : \
                                      (p_video_dev->pixel_format == V4L2_PIX_FMT_RGB565) ? 16 : 0;
    p_video_buf->pixel_datas.line_bytes       = p_video_dev->width * p_video_buf->pixel_datas.bpp / 8;
    p_video_buf->pixel_datas.total_bytes      = ret; //数据有多少字节
    p_video_buf->pixel_datas.pixel_datas_addr = p_video_dev->video_buf[0];

    return 0;
}

static int v4l2_put_frame_readwrite(p_video_device p_video_dev, p_video_buffer p_video_buf)
{
    //No operation
    return 0;
}


static int v4l2_start_device(p_video_device p_video_dev)
{
    int ret;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    ret = ioctl(p_video_dev->fd, VIDIOC_STREAMON, &type);
    if (0 != ret)
    {
        printf_debug("Can't VIDIOC_STREAMON device\n");
        return -1;
    }

    return 0;
}


static int v4l2_stop_device(p_video_device p_video_dev)
{
    int ret;
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    ret = ioctl(p_video_dev->fd, VIDIOC_STREAMOFF, &type);
    if (0 != ret)
    {
        printf_debug("Can't VIDIOC_STREAMOFF device\n");
        return -1;
    }

    return 0;
}


//构造一个video_operations结构体
static video_operations v4l2_video_ops =
{
    .name        = "v4l2",

    .init_device  = v4l2_init_device,
    .exit_device  = v4l2_exit_device,

    .get_format   = v4l2_get_format,

    .get_frame    = v4l2_get_frame_streaming,
    .put_frame    = v4l2_put_frame_streaming,

    .start_device = v4l2_start_device,
    .stop_device  = v4l2_stop_device,
};

/* 注册这个结构体 */
int v4l2_init(void)
{
    return register_video_ops(&v4l2_video_ops);
}


