#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <linux/version.h>
#include <asm/unaligned.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/videodev2.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-event.h>
#include <media/v4l2-device.h>
#include <media/videobuf-core.h>
#include <media/videobuf-vmalloc.h>
#include <media/v4l2-common.h>

#include "uvcvideo.h"


/* Values for bmHeaderInfo (Video and Still Image Payload Headers, 2.4.3.3) */
#define UVC_STREAM_EOH	(1 << 7)
#define UVC_STREAM_ERR	(1 << 6)
#define UVC_STREAM_STI	(1 << 5)
#define UVC_STREAM_RES	(1 << 4)
#define UVC_STREAM_SCR	(1 << 3)
#define UVC_STREAM_PTS	(1 << 2)
#define UVC_STREAM_EOF	(1 << 1)
#define UVC_STREAM_FID	(1 << 0)

/* 参考uvc_video_queue定义一些结构体 */
struct my_uvc_buffer
{
    struct v4l2_buffer buf;
    int state;
    int vma_use_count;         //表示是否已经被mmap
    wait_queue_head_t wait;    //APP要读某个缓冲区,如果无数据,在此休眠
    struct list_head stream;
    struct list_head irq;
};

struct my_uvc_queue
{
    void *mem;                 //整块内存的起始地址
    int count;                 //队列分配了几个buf
    int buf_size;              //每个buf大小
    struct my_uvc_buffer buffer[32];

    struct urb *urb[32];       //32个urb
    char *urb_buffer[32];      //分配的urb buffer
    dma_addr_t urb_dma[32];    //urb buffer的dma
    unsigned int urb_size;     //urb大小

    struct list_head mainqueue; //供APP读取数据用
    struct list_head irqqueue;  //供底层驱动产生数据用
};

static struct my_uvc_queue my_uvc_q;

static struct v4l2_format my_uvc_format;

//用于保存从USB获取的摄像头参数
struct my_uvc_streaming_control
{
    __u16 bmHint;
    __u8  bFormatIndex;
    __u8  bFrameIndex;
    __u32 dwFrameInterval;
    __u16 wKeyFrameRate;
    __u16 wPFrameRate;
    __u16 wCompQuality;
    __u16 wCompWindowSize;
    __u16 wDelay;
    __u32 dwMaxVideoFrameSize;
    __u32 dwMaxPayloadTransferSize;
    __u32 dwClockFrequency;
    __u8  bmFramingInfo;
    __u8  bPreferedVersion;
    __u8  bMinVersion;
    __u8  bMaxVersion;
};

static struct my_uvc_streaming_control my_uvc_params;
static struct usb_device *my_uvc_udev;

#define MY_UVC_URBS_NUM  2  //限制最多2个urb,一个也行
static int my_uvc_streaming_intf; //VS
static int my_uvc_control_intf;   //VC

//摄像头硬件相关参数:lsusb得到
#define MY_UVC_FMT  "MJPEG"                //bDescriptorSubtype:FORMAT_MJPEG;对应MJPEG
static int my_uvc_bEndpointAddress = 0x82; //用于指定后面传数据的端点L:Video Streaming的端点地址
static int my_uvc_wWidth  = 640;           //支持640*480、320*240、160*120
static int my_uvc_wHeight = 480;
static int my_uvc_bcdUVC  = 0x100;         //bcdUVC=1.00;再BCD转换,eg:2.10 -> 210H, 1.50 -> 150H
static int my_uvc_bUnitID = 3;             //用于亮度控制,摄像头有多个UNIT,这里使用PROCESSING_UNIT,对应的ID是3;
static int my_uvc_wMaxPacketSize = 956;    //my_uvc_params得知:不同分辨率,该值不一样;
static int my_uvc_bAlternateSetting = 6;   //my_uvc_wMaxPacketSize对应的带宽节点bAlternateSetting
static int dwMaxVideoFrameSize = 307712;   //my_uvc_params得知:实测为my_uvc_wWidth*my_uvc_wHeight多512;
static unsigned char bFrameIndex = 1;      //使用第二种分辨率:640x480(1),320x240(2),160x120(3)
//static unsigned int my_uvc_bBitsPerPixel = 16; //本USB摄像头传输的是压缩数据,不存在该参数


/* v4l2_file_operations */
static int my_uvc_open(struct file *file)
{
    printk("enter %s\n", __func__);

    return 0;
}

//把缓存映射到APP的空间,以后APP就可以直接操作这块缓存
static int my_uvc_mmap(struct file *file, struct vm_area_struct *vma)
{
    int i, ret = 0;
    struct page *page;
    struct my_uvc_buffer *buffer;
    unsigned long addr, start, size;
    
    printk("enter %s\n", __func__);

    start = vma->vm_start;
    size = vma->vm_end - vma->vm_start;

    //应用程序调用mmap函数时,会传入offset参数,再根据offset找出指定的缓冲区
    for (i = 0; i < my_uvc_q.count; ++i)
    {
        buffer = &my_uvc_q.buffer[i];
        if ((buffer->buf.m.offset >> PAGE_SHIFT) == vma->vm_pgoff)
            break;
    }

    //没找到对应的my_uvc_q.buffer或大小不对
    if ((i == my_uvc_q.count) || (size != my_uvc_q.buf_size))
        return -EINVAL;

    /* VM_IO marks the area as being an mmaped region for I/O to a
     * device. It also prevents the region from being core dumped. */
    vma->vm_flags |= VM_IO;

    //根据虚拟地址得到缓冲区对应的page结构体
    addr = (unsigned long)my_uvc_q.mem + buffer->buf.m.offset;
    while (size > 0) //循环把size大小的空间变为page
    {
        page = vmalloc_to_page((void *)addr);

        //把page和APP传入的虚拟地址挂构
        if ((ret = vm_insert_page(vma, start, page)) < 0)
            return ret ;

        start += PAGE_SIZE;
        addr  += PAGE_SIZE;
        size  -= PAGE_SIZE;
    }

    buffer->vma_use_count++; //引用计数+1

    return ret;
}

//APP调用POLL/select来确定缓存是否就绪(有数据)
static unsigned int my_uvc_poll(struct file *file, struct poll_table_struct *wait)
{
    struct my_uvc_buffer *buf;
    unsigned int mask = 0;

    printk("enter %s\n", __func__);

    //从mainqueuq中取出第1个缓冲区,判断它的状态, 如果未就绪,休眠
    if (list_empty(&my_uvc_q.mainqueue))
    {
        mask |= POLLERR;
        return mask;
    }

    buf = list_first_entry(&my_uvc_q.mainqueue, struct my_uvc_buffer, stream);

    poll_wait(file, &buf->wait, wait);
    if (buf->state == VIDEOBUF_DONE || buf->state == VIDEOBUF_ERROR)
        mask |= POLLIN | POLLRDNORM; //普通或优先级带数据可读 | 普通数据可读
        
    return mask;
}


/* v4l2_ioctl_ops */
static int my_uvc_vidioc_querycap(struct file *file, void  *priv, struct v4l2_capability *cap)
{
    struct video_device *vdev = video_devdata(file);

    printk("enter %s\n", __func__);

    strlcpy(cap->driver, "my_uvc_video", sizeof(cap->driver));
    strlcpy(cap->card, vdev->name, sizeof(cap->card));

    cap->version = 4;

    cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS;
    cap->device_caps  = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS;

    return 0;
}
static int my_uvc_vidioc_enum_fmt_vid_cap(struct file *file, void  *priv, struct v4l2_fmtdesc *f)
{
    printk("enter %s\n", __func__);

    /* 根据摄像头的设备描述符可知,只支持一种格式:VS_FORMAT_MJPEG */
    if(f->index >= 1)
        return -EINVAL;

    strcpy(f->description, MY_UVC_FMT); //支持格式
    f->pixelformat = V4L2_PIX_FMT_MJPEG;
    f->type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    return 0;
}

static int my_uvc_vidioc_g_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
    printk("enter %s\n", __func__);

    memcpy(f, &my_uvc_format, sizeof(my_uvc_format));

    return 0;
}

static int my_uvc_vidioc_try_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
    printk("enter %s\n", __func__);

    if ((f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) || (f->fmt.pix.pixelformat != V4L2_PIX_FMT_MJPEG))
        return -EINVAL;

    /* 调整format的width, height */
    f->fmt.pix.width  = my_uvc_wWidth; //设备描述符里支持的分辨率:640x480,320x240,160x120
    f->fmt.pix.height = my_uvc_wHeight;

    f->fmt.pix.field      = V4L2_FIELD_NONE;

    /* 计算bytesperline, sizeimage */
    //bBitsPerPixel = my_uvc_bBitsPerPixel; //lsusb:bBitsPerPixel
    //f->fmt.pix.bytesperline = (f->fmt.pix.width * bBitsPerPixel) >> 3;
    f->fmt.pix.sizeimage = dwMaxVideoFrameSize; //f->fmt.pix.height * f->fmt.pix.bytesperline;

    f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
    f->fmt.pix.priv       = 0;		//private data, depends on pixelformat

    return 0;
}

static int my_uvc_vidioc_s_fmt_vid_cap(struct file *file, void *priv, struct v4l2_format *f)
{
    int ret;

    printk("enter %s\n", __func__);

    ret = my_uvc_vidioc_try_fmt_vid_cap(file, NULL, f);
    if (ret < 0)
        return ret;

    memcpy(&my_uvc_format, f, sizeof(my_uvc_format));

    return 0;
}

/* APP调用该ioctl让驱动程序分配若干个buf, APP将从这些buf中读到视频数据  */
static int my_uvc_vidioc_reqbufs(struct file *file, void *priv, struct v4l2_requestbuffers *p)
{
    unsigned int i;
    void *mem = NULL;
    int nbuffers = p->count; //buf数量
    int bufsize  = PAGE_ALIGN(my_uvc_format.fmt.pix.sizeimage); //buf大小,且长度页对齐

    printk("enter %s\n", __func__);

    if (my_uvc_q.mem)    //如果原来分配了buf,先释放原来的buf
    {
        vfree(my_uvc_q.mem);
        memset(&my_uvc_q, 0, sizeof(my_uvc_q));
        my_uvc_q.mem = NULL;
    }

    if (nbuffers == 0)   //没有需要分配的,直接退出
        return 0;

    for (; nbuffers > 0; --nbuffers)          //依次减少buf数量，直到分配成功
    {
        mem = vmalloc_32(nbuffers * bufsize); //这些buf是一次性作为一个整体来分配的
        if (mem != NULL)
            break;
    }

    if (mem == NULL)
        return -ENOMEM;

    memset(&my_uvc_q, 0, sizeof(my_uvc_q)); //清空my_uvc_q,初始化

    INIT_LIST_HEAD(&my_uvc_q.mainqueue); //初始化两个队列,my_uvc_vidioc_qbuf
    INIT_LIST_HEAD(&my_uvc_q.irqqueue);

    for (i = 0; i < nbuffers; ++i)
    {
        my_uvc_q.buffer[i].buf.index    = i;  //索引
        my_uvc_q.buffer[i].buf.m.offset = i * bufsize; //偏移
        my_uvc_q.buffer[i].buf.length   = my_uvc_format.fmt.pix.sizeimage; //原始大小;实测PAGE_ALIGN对齐,也没问题
        my_uvc_q.buffer[i].buf.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE; //视频捕获设备
        my_uvc_q.buffer[i].buf.sequence = 0;
        my_uvc_q.buffer[i].buf.field    = V4L2_FIELD_NONE;
        my_uvc_q.buffer[i].buf.memory   = V4L2_MEMORY_MMAP;
        my_uvc_q.buffer[i].buf.flags    = 0;
        my_uvc_q.buffer[i].state        = VIDEOBUF_IDLE; //分配完更新状态为空闲
        init_waitqueue_head(&my_uvc_q.buffer[i].wait); //初始化一个等待队列
    }

    my_uvc_q.mem = mem;
    my_uvc_q.count = nbuffers;
    my_uvc_q.buf_size = bufsize;

    return nbuffers;
}

/* 查询缓存状态, 比如地址信息(APP可以用mmap进行映射)  */
static int my_uvc_vidioc_querybuf(struct file *file, void *priv, struct v4l2_buffer *v4l2_buf)
{
    int ret = 0;

    printk("enter %s\n", __func__);

    if (v4l2_buf->index >= my_uvc_q.count)
    {
        ret = -EINVAL;
        goto done;
    }

    memcpy(v4l2_buf, &my_uvc_q.buffer[v4l2_buf->index].buf, sizeof(*v4l2_buf));

    if (my_uvc_q.buffer[v4l2_buf->index].vma_use_count) //更新flags
        v4l2_buf->flags |= V4L2_BUF_FLAG_MAPPED;

#if 0
    switch (my_uvc_q.buffer[v4l2_buf->index].state)    //将uvc flags转换成V4L2 flags
    {
        case VIDEOBUF_ERROR:
        case VIDEOBUF_DONE:
            v4l2_buf->flags |= V4L2_BUF_FLAG_DONE;
            break;
        case VIDEOBUF_QUEUED:
        case VIDEOBUF_ACTIVE:
            v4l2_buf->flags |= V4L2_BUF_FLAG_QUEUED;
            break;
        case VIDEOBUF_IDLE:
        default:
            break;
    }
#endif

done:
    return ret;
}

/* 把传入的缓冲区放入队列, 底层的硬件操作函数将会把数据放入这个队列的缓存 */
static int my_uvc_vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *v4l2_buf)
{
    printk("enter %s\n", __func__);

    /* 0. APP传入的v4l2_buf可能有问题, 要做判断 */
    if (v4l2_buf->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || v4l2_buf->memory != V4L2_MEMORY_MMAP)
        return -EINVAL;

    if (v4l2_buf->index >= my_uvc_q.count)
        return -EINVAL;

    if (my_uvc_q.buffer[v4l2_buf->index].state != VIDEOBUF_IDLE)
        return -EINVAL;

    /* 1. 修改状态 */
    my_uvc_q.buffer[v4l2_buf->index].state = VIDEOBUF_QUEUED;
    my_uvc_q.buffer[v4l2_buf->index].buf.bytesused = 0;

    /* 2. 放入2个队列 */
    //队列1: 供应用层使用
    //当队列中缓冲区有数据时, 应用层从mainqueue队列中取出数据
    list_add_tail(&my_uvc_q.buffer[v4l2_buf->index].stream, &my_uvc_q.mainqueue);

    //队列2: 供产生数据的函数使用
    //当采集到数据时,从irqqueue队列中取出首个缓冲区,存入数据
    list_add_tail(&my_uvc_q.buffer[v4l2_buf->index].irq, &my_uvc_q.irqqueue);

    return 0;
}

/* APP通过poll/select确定有数据后,把buf从mainqueue队列中取出来 */
static int my_uvc_vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *v4l2_buf)
{
    struct my_uvc_buffer *get_buf;

    printk("enter %s\n", __func__);

    if (list_empty(&my_uvc_q.mainqueue))
        return -EINVAL;

    get_buf = list_first_entry(&my_uvc_q.mainqueue, struct my_uvc_buffer, stream); //取出buf

    switch (get_buf->state)   //修改状态
    {
        case VIDEOBUF_ERROR:
            return -EIO;
        case VIDEOBUF_DONE:
            get_buf->state = VIDEOBUF_IDLE;
            break;
        case VIDEOBUF_IDLE:
        case VIDEOBUF_QUEUED:
        case VIDEOBUF_ACTIVE:
        default:
            return -EINVAL;
    }

    list_del(&get_buf->stream); //从队列删除
    memcpy(v4l2_buf, &get_buf->buf, sizeof *v4l2_buf); //复制返回数据

    return 0;
}

/* Extract the bit string specified by mapping->offset and mapping->size
 * from the little-endian data stored at 'data' and return the result as
 * a signed 32bit integer. Sign extension will be performed if the mapping
 * references a signed data type.*/
static __s32 my_uvc_get_le_value(const __u8 *data)
{
    int bits = 16;  //uvc_ctrl_mappings中.size = 16
    int offset = 0; //uvc_ctrl_mappings中.offset = 0
    __s32 value = 0;
    __u8 mask;

    data += offset / 8;
    offset &= 7;
    mask = ((1LL << bits) - 1) << offset;

    for (; bits > 0; data++)
    {
        __u8 byte = *data & mask;
        value |= offset > 0 ? (byte >> offset) : (byte << (-offset));
        bits -= 8 - (offset > 0 ? offset : 0);
        offset -= 8;
        mask = (1 << bits) - 1;
    }

    /* Sign-extend the value if needed. */
    //uvc_ctrl_mappings中.data_type = UVC_CTRL_DATA_TYPE_SIGNED
    value |= -(value & (1 << (16 - 1))); //uvc_ctrl_mappings中.size = 16

    return value;
}

/* Set the bit string specified by mapping->offset and mapping->size
 * in the little-endian data stored at 'data' to the value 'value'.*/
static void my_uvc_set_le_value(__s32 value, __u8 *data)
{
    int bits = 16;  //uvc_ctrl_mappings中.size = 16
    int offset = 0; //uvc_ctrl_mappings中.offset = 0
    __u8 mask;

    data += offset / 8;
    offset &= 7;

    for (; bits > 0; data++)
    {
        mask = ((1LL << bits) - 1) << offset;
        *data = (*data & ~mask) | ((value << offset) & mask);
        value >>= offset ? offset : 8;
        bits -= 8 - offset;
        offset = 0;
    }
}

int my_uvc_vidioc_queryctrl (struct file *file, void *fh, struct v4l2_queryctrl *ctrl)
{
    unsigned char data[2];  

    printk("enter %s\n", __func__);

    if (ctrl->id != V4L2_CID_BRIGHTNESS)     //这里只操作控制亮度的v4l2_queryctrl
        return -EINVAL;

    memset(ctrl, 0, sizeof * ctrl);          //初始化,清空
    ctrl->id   = V4L2_CID_BRIGHTNESS;        //设置ID
    ctrl->type = V4L2_CTRL_TYPE_INTEGER;     //设置属性类别(整数)
    strcpy(ctrl->name, "MY_UVC_BRIGHTNESS"); //设置名字
    ctrl->flags = 0;                         //默认支持设置等

    /* 发起USB传输,从摄像头获取这些值 */
    //设置最小值
    if(2 != usb_control_msg(my_uvc_udev, usb_rcvctrlpipe(my_uvc_udev, 0), 
                            GET_MIN, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN, 
                            PU_BRIGHTNESS_CONTROL << 8, my_uvc_bUnitID << 8 | my_uvc_control_intf, data, 2, 5000)) 
        return -EIO;
    ctrl->minimum = my_uvc_get_le_value(data);	

    //设置最大值
    if(2 != usb_control_msg(my_uvc_udev, usb_rcvctrlpipe(my_uvc_udev, 0), 
                            GET_MAX, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN, 
                            PU_BRIGHTNESS_CONTROL << 8, my_uvc_bUnitID << 8 | my_uvc_control_intf, data, 2, 5000))
        return -EIO;
    ctrl->maximum = my_uvc_get_le_value(data);	

    //设置步长
    if(2 != usb_control_msg(my_uvc_udev, usb_rcvctrlpipe(my_uvc_udev, 0), 
                            GET_RES, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN, 
                            PU_BRIGHTNESS_CONTROL << 8, my_uvc_bUnitID << 8 | my_uvc_control_intf, data, 2, 5000))
        return -EIO;
    ctrl->step = my_uvc_get_le_value(data);	

    //设置典型值
    if(2 != usb_control_msg(my_uvc_udev, usb_rcvctrlpipe(my_uvc_udev, 0), 
                            GET_DEF, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN, 
                            PU_BRIGHTNESS_CONTROL << 8, my_uvc_bUnitID << 8 | my_uvc_control_intf, data, 2, 5000))
        return -EIO;
    ctrl->default_value = my_uvc_get_le_value(data);	

    printk("Brightness: min =%d, max = %d, step = %d, default = %d\n", ctrl->minimum, ctrl->maximum, ctrl->step, ctrl->default_value);

    return 0;
}

int my_uvc_vidioc_g_ctrl (struct file *file, void *fh, struct v4l2_control *ctrl)
{
    unsigned char data[2];

    printk("enter %s\n", __func__);

    if (ctrl->id != V4L2_CID_BRIGHTNESS)
        return -EINVAL;

    if(2 != usb_control_msg(my_uvc_udev, usb_rcvctrlpipe(my_uvc_udev, 0), 
                            GET_CUR, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN, 
                            PU_BRIGHTNESS_CONTROL << 8, my_uvc_bUnitID << 8 | my_uvc_control_intf, data, 2, 5000))
        return -EIO;
    
    ctrl->value = my_uvc_get_le_value(data);	

    return 0;
}

int my_uvc_vidioc_s_ctrl (struct file *file, void *fh, struct v4l2_control *ctrl)
{
    unsigned char data[2];

    printk("enter %s\n", __func__);

    if (ctrl->id != V4L2_CID_BRIGHTNESS)
        return -EINVAL;

    my_uvc_set_le_value(ctrl->value, data);

    if(2 != usb_control_msg(my_uvc_udev, usb_sndctrlpipe(my_uvc_udev, 0), 
                            SET_CUR, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT, 
                            PU_BRIGHTNESS_CONTROL << 8, my_uvc_bUnitID  << 8 | my_uvc_control_intf, data, 2, 5000))
        return -EIO;

    return 0;
}

static void my_uvc_video_complete(struct urb *urb)
{
    int ret, i;
    unsigned char *mem;
    unsigned char *src, *dest;
    struct my_uvc_buffer *buf;
    int len, maxlen, nbytes, data_len;
    
    static int fid, last_fid = -1;

    //要修改影像資料，必須先宣告一個特別型態的指標變數，才能正確存取記憶體中的資料
    unsigned char *point_mem;
    static unsigned char *mem_temp = NULL;

    //初始化暫存用的記憶體位置
    static unsigned int nArrayTemp_Size = 1000;

    printk("enter %s\n", __func__);
    //printk("=======urb->status: %d ======\n", urb->status);
    
	switch (urb->status) {
        case 0:             //Success 
            break;
        case -ETIMEDOUT:    //Nak
        case -ECONNRESET:   //Kill
        case -ENOENT:
        case -ESHUTDOWN:
        default:            //Error
            return;
	}

    /* 从irqqueue队列中取出首个缓冲区 */
    if (!list_empty(&my_uvc_q.irqqueue)) //判断是不是空队列
        buf = list_first_entry(&my_uvc_q.irqqueue, struct my_uvc_buffer, irq);//取出首buf用于后续存放数据
    else
        buf = NULL;

    for (i = 0; i < urb->number_of_packets; ++i) //一次urb传输包含number_of_packets个子包
    {
        if (urb->iso_frame_desc[i].status < 0)
            continue;
        
        src = urb->transfer_buffer + urb->iso_frame_desc[i].offset; //数据源
        len = urb->iso_frame_desc[i].actual_length; //数据长度
        if(buf)
            dest = my_uvc_q.mem + buf->buf.m.offset + buf->buf.bytesused; //目的地址

        //判断数据是否有效;URB数据含义: data[0]->头部长度;data[1]->错误状态
        if ((len < 2) || (src[0] < 2) || (src[0] > len) || (src[1] & UVC_STREAM_ERR))
            continue;
        
        if (my_uvc_udev->descriptor.idVendor == 0x1B3B) /* ip2970/ip2977 */
        {
            if ( len >= 16 ) // have data in buffer
            {
                // 資料必須從data[12]開始判斷，是因為前面的資料是封包專用
                if ( (src[12] == 0xFF && src[13] == 0xD8 && src[14] == 0xFF) ||
                        (src[12] == 0xD8 && src[13] == 0xFF && src[14] == 0xC4))
                {
                    if(last_fid) //效果:取反
                        fid &= ~UVC_STREAM_FID; 
                    else
                        fid |= UVC_STREAM_FID;
                }
            }
        }
        else
        {
            fid = src[1] & UVC_STREAM_FID;
        }

        /* Store the payload FID bit and return immediately when the buffer is NULL.*/
        if (buf == NULL)
        {
            last_fid = fid;//?必要性？
            continue;
        }

        if (buf->state != VIDEOBUF_ACTIVE)  //!= VIDEOBUF_ACTIVE, 表示"之前还未接收数据" 
        {
            if (fid == last_fid)
                continue; //因为是第一次接收数据,前面的fid已经被取反,不该等于上一次的last_fid
            buf->state = VIDEOBUF_ACTIVE; //表示开始接收第1个数据
        }

        last_fid = fid; //开始传本帧数据

        len -= src[0]; //除去头部后的数据长度
        maxlen = buf->buf.length - buf->buf.bytesused; //缓冲区最多还能存多少数据
        nbytes = min(len, maxlen);

        memcpy(dest, src + src[0], nbytes); //复制数据
        
        buf->buf.bytesused += nbytes; //更新buf已使用空间

        /* ip2970/ip2977 */
        if (my_uvc_udev->descriptor.idVendor == 0x1B3B)
        {
            if(mem_temp == NULL)
            {
                mem_temp = kmalloc(nArrayTemp_Size, GFP_KERNEL);
            }
            else if(nArrayTemp_Size <= nbytes)  //當收到的資料長度大於上一次的資料長度，則重新分配所需的空間+
            {
                kfree(mem_temp);
                nArrayTemp_Size += 500;
                mem_temp = kmalloc(nArrayTemp_Size, GFP_KERNEL);
            }
            memset(mem_temp, 0x00, nArrayTemp_Size);

            // 指向資料儲存的記憶體位置
            point_mem = (unsigned char *)dest;
            if( *(point_mem) == 0xD8 && *(point_mem + 1) == 0xFF && *(point_mem + 2) == 0xC4)
            {
                memcpy( mem_temp + 1, point_mem, nbytes);
                mem_temp[0] = 0xFF;
                memcpy(point_mem, mem_temp, nbytes + 1);
            }
        }

        /* 判断一帧数据是否已经全部接收到 */
        if (len > maxlen)
            buf->state = VIDEOBUF_DONE;

        /* Mark the buffer as done if the EOF marker is set. */
        if ((src[1] & UVC_STREAM_EOF) && (buf->buf.bytesused != 0))
            buf->state = VIDEOBUF_DONE;

        /* 当接收完一帧数据,从irqqueue中删除这个缓冲区,唤醒等待数据的进程 */
        if ((buf->state == VIDEOBUF_DONE) || (buf->state == VIDEOBUF_ERROR))
        {
            list_del(&buf->irq);
            wake_up(&buf->wait);

            mem = my_uvc_q.mem + buf->buf.m.offset;
            data_len = buf->buf.bytesused;
  
            /* 取出下一个buf */
            if (!list_empty(&my_uvc_q.irqqueue))
                buf = list_first_entry(&my_uvc_q.irqqueue, struct my_uvc_buffer, irq);
            else
                buf = NULL;
        }

    }
    
    /* 再次提交URB */
    if ((ret = usb_submit_urb(urb, GFP_ATOMIC)) < 0)
    {
        printk("Failed to resubmit video URB (%d).\n", ret);
    }
}

static void my_uvc_uninit_urbs(void)
{
    unsigned int i;

    for (i = 0; i < MY_UVC_URBS_NUM; ++i)   
    {
        //释放usb_buffers
        //同时判断urb大小,如果非0才执行,因为本函数最后会将其置0,streamoff调用时,就不应该再释放一次
        if (my_uvc_q.urb_buffer[i] && my_uvc_q.urb_size)
        {
            usb_free_coherent(my_uvc_udev, my_uvc_q.urb_size, my_uvc_q.urb_buffer[i], my_uvc_q.urb_dma[i]);
            my_uvc_q.urb_buffer[i] = NULL;
        }

        //释放urb
        if (my_uvc_q.urb[i])   
        {
            usb_free_urb(my_uvc_q.urb[i]);
            my_uvc_q.urb[i] = NULL;
        }
    }
    my_uvc_q.urb_size = 0;
}


static int my_uvc_alloc_init_urbs(void)
{
    int i, j;
    int npackets;
    unsigned int size;
    unsigned short psize;
    
    struct urb *urb;

    psize = my_uvc_wMaxPacketSize; //实时传输端点一次能传输的最大字节数;lsusb: wMaxPacketSize
    size  = my_uvc_params.dwMaxVideoFrameSize; //一帧数据的最大大小
    npackets = DIV_ROUND_UP(size, psize); //传多少次(向上取整)
    if (npackets == 0)
        return -ENOMEM;
    
    size = my_uvc_q.urb_size = psize * npackets; //取整后新大小

    for (i = 0; i < MY_UVC_URBS_NUM; ++i)
    {
        /* 1.分配usb_buffers */
        my_uvc_q.urb_buffer[i] = usb_alloc_coherent(my_uvc_udev, size, GFP_KERNEL | __GFP_NOWARN, &my_uvc_q.urb_dma[i]);
        /* 2.分配urb */
        my_uvc_q.urb[i] = usb_alloc_urb(npackets, GFP_KERNEL);

        if (!my_uvc_q.urb_buffer[i] || !my_uvc_q.urb[i]) //如果分配失败
        {
            my_uvc_uninit_urbs();

            return -ENOMEM;
        }
    }

    /* 3. 设置urb */
    for (i = 0; i < MY_UVC_URBS_NUM; ++i)
    {
        urb = my_uvc_q.urb[i];

        urb->dev = my_uvc_udev;
        urb->pipe = usb_rcvisocpipe(my_uvc_udev, my_uvc_bEndpointAddress); //lsusb: bEndpointAddress 0x82
        urb->transfer_flags = URB_ISO_ASAP | URB_NO_TRANSFER_DMA_MAP;
        urb->interval = 1; //lsusb: bInterval 1
        urb->transfer_buffer = my_uvc_q.urb_buffer[i];
        urb->transfer_dma = my_uvc_q.urb_dma[i];
        urb->complete = my_uvc_video_complete; //中断处理函数
        urb->number_of_packets = npackets;
        urb->transfer_buffer_length = size;

        for (j = 0; j < npackets; ++j)
        {
            urb->iso_frame_desc[j].offset = j * psize;
            urb->iso_frame_desc[j].length = psize;
        }
    }

    return 0;
}


static void ctrl_to_data(struct my_uvc_streaming_control *ctrl, unsigned char *data, unsigned short size)
{
    *(__le16 *)&data[0] = cpu_to_le16(ctrl->bmHint);
    data[2] = ctrl->bFormatIndex;
    data[3] = ctrl->bFrameIndex;
    *(__le32 *)&data[4] = cpu_to_le32(ctrl->dwFrameInterval);
    *(__le16 *)&data[8] = cpu_to_le16(ctrl->wKeyFrameRate);
    *(__le16 *)&data[10] = cpu_to_le16(ctrl->wPFrameRate);
    *(__le16 *)&data[12] = cpu_to_le16(ctrl->wCompQuality);
    *(__le16 *)&data[14] = cpu_to_le16(ctrl->wCompWindowSize);
    *(__le16 *)&data[16] = cpu_to_le16(ctrl->wDelay);
    put_unaligned_le32(ctrl->dwMaxVideoFrameSize, &data[18]);
    put_unaligned_le32(ctrl->dwMaxPayloadTransferSize, &data[22]);

    if (size == 34)
    {
        put_unaligned_le32(ctrl->dwClockFrequency, &data[26]);
        data[30] = ctrl->bmFramingInfo;
        data[31] = ctrl->bPreferedVersion;
        data[32] = ctrl->bMinVersion;
        data[33] = ctrl->bMaxVersion;
    }
}

static void data_to_ctrl(unsigned char *data, struct my_uvc_streaming_control *ctrl, unsigned short size)
{
    ctrl->bmHint = le16_to_cpup((__le16 *)&data[0]);
    ctrl->bFormatIndex = data[2];
    ctrl->bFrameIndex = data[3];
    ctrl->dwFrameInterval = le32_to_cpup((__le32 *)&data[4]);
    ctrl->wKeyFrameRate = le16_to_cpup((__le16 *)&data[8]);
    ctrl->wPFrameRate = le16_to_cpup((__le16 *)&data[10]);
    ctrl->wCompQuality = le16_to_cpup((__le16 *)&data[12]);
    ctrl->wCompWindowSize = le16_to_cpup((__le16 *)&data[14]);
    ctrl->wDelay = le16_to_cpup((__le16 *)&data[16]);
    ctrl->dwMaxVideoFrameSize = get_unaligned_le32(&data[18]);
    ctrl->dwMaxPayloadTransferSize = get_unaligned_le32(&data[22]);

    if (size == 34)
    {
        ctrl->dwClockFrequency = get_unaligned_le32(&data[26]);
        ctrl->bmFramingInfo = data[30];
        ctrl->bPreferedVersion = data[31];
        ctrl->bMinVersion = data[32];
        ctrl->bMaxVersion = data[33];
    }
    else
    {
        //ctrl->dwClockFrequency = video->dev->clock_frequency;
        ctrl->bmFramingInfo = 0;
        ctrl->bPreferedVersion = 0;
        ctrl->bMinVersion = 0;
        ctrl->bMaxVersion = 0;
    }
}


static int my_uvc_try_streaming_params(struct my_uvc_streaming_control *ctrl)
{
    int ret = 0;
    unsigned char *data;
    unsigned short size;

    //lsusb得到:bcdUVC =   1.00;再BCD转换,eg:2.10 -> 210H, 1.50 -> 150H
    size = my_uvc_bcdUVC >= 0x0110 ? 34 : 26; //根据版本分配buf大小
    data = kmalloc(size, GFP_KERNEL);
    if (data == NULL)
        return -ENOMEM;

    memset(ctrl, 0, sizeof * ctrl);

    ctrl->bmHint = 1;	    //保持dwFrameInterval不变    
    ctrl->bFormatIndex = 1; //支持格式数量
    ctrl->bFrameIndex  = bFrameIndex; //使用第二种分辨率:640x480(1),320x240(2),160x120(3)
    ctrl->dwFrameInterval = 333333;   //lsusb: dwFrameInterval(0)  333333 每秒30帧

    ctrl_to_data(ctrl, data, size);

    //通过usb尝试设置摄像头参数
    ret = usb_control_msg(my_uvc_udev,  usb_sndctrlpipe(my_uvc_udev, 0),
                          SET_CUR, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
                          VS_PROBE_CONTROL << 8, 0 << 8 | my_uvc_streaming_intf, data, size, 5000); 
    kfree(data);

    return ret;
}

static int my_uvc_get_streaming_params(struct my_uvc_streaming_control *ctrl)
{
    int ret = 0;
    unsigned char *data;
    unsigned short size;

    //lsusb得到:bcdUVC=1.00;再BCD转换,eg:2.10 -> 210H, 1.50 -> 150H
    size = my_uvc_bcdUVC >= 0x0110 ? 34 : 26; //根据版本分配buf大小
    data = kmalloc(size, GFP_KERNEL);
    if (data == NULL)
        return -ENOMEM;

    //通过usb获取摄像头参数
    ret = usb_control_msg(my_uvc_udev, usb_rcvctrlpipe(my_uvc_udev, 0),
                          GET_CUR, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
                          VS_PROBE_CONTROL << 8, 0 << 8 | my_uvc_streaming_intf, data, size, 5000); 
    if (ret < 0)
        goto done;

    //返回摄像头参数
    data_to_ctrl(data, ctrl, size);

done:
    kfree(data);

    return ret;
}

static int my_uvc_set_streaming_params(struct my_uvc_streaming_control *ctrl)
{
    int ret = 0;
    unsigned char *data;
    unsigned short size;

    //lsusb得到:bcdUVC=1.00;再BCD转换,eg:2.10 -> 210H, 1.50 -> 150H
    size = my_uvc_bcdUVC >= 0x0110 ? 34 : 26; //根据版本分配buf大小
    data = kmalloc(size, GFP_KERNEL);
    if (data == NULL)
        return -ENOMEM;

    ctrl_to_data(ctrl, data, size);

    //通过usb尝试设置摄像头参数
    ret = usb_control_msg(my_uvc_udev,  usb_sndctrlpipe(my_uvc_udev, 0),
                          SET_CUR, USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_OUT,
                          VS_COMMIT_CONTROL << 8, 0 << 8 | my_uvc_streaming_intf, data, size, 5000); 
    kfree(data);

    return ret;
}


static void my_uvc_printk_streaming_params(void)
{
    int ret;
#if 1  
	ret = my_uvc_try_streaming_params(&my_uvc_params); //测试参数
	printk("ret1 = %d\n", ret);

	ret = my_uvc_get_streaming_params(&my_uvc_params); //取出参数
	printk("ret2 = %d\n", ret);

	ret = my_uvc_set_streaming_params(&my_uvc_params); //设置参数
	printk("ret3 = %d\n", ret);
#endif

    printk("video params:\n");
    printk("bmHint                   = %d\n", my_uvc_params.bmHint);
    printk("bFormatIndex             = %d\n", my_uvc_params.bFormatIndex);
    printk("bFrameIndex              = %d\n", my_uvc_params.bFrameIndex);
    printk("dwFrameInterval          = %d\n", my_uvc_params.dwFrameInterval);
    printk("wKeyFrameRate            = %d\n", my_uvc_params.wKeyFrameRate);
    printk("wPFrameRate              = %d\n", my_uvc_params.wPFrameRate);
    printk("wCompQuality             = %d\n", my_uvc_params.wCompQuality);
    printk("wCompWindowSize          = %d\n", my_uvc_params.wCompWindowSize);
    printk("wDelay                   = %d\n", my_uvc_params.wDelay);
    printk("dwMaxVideoFrameSize      = %d\n", my_uvc_params.dwMaxVideoFrameSize); //得知dwMaxVideoFrameSize
    printk("dwMaxPayloadTransferSize = %d\n", my_uvc_params.dwMaxPayloadTransferSize); //得知dwMaxPayloadTransferSize
    printk("dwClockFrequency         = %d\n", my_uvc_params.dwClockFrequency);
    printk("bmFramingInfo            = %d\n", my_uvc_params.bmFramingInfo);
    printk("bPreferedVersion         = %d\n", my_uvc_params.bPreferedVersion);
    printk("bMinVersion              = %d\n", my_uvc_params.bMinVersion);
    printk("bMinVersion              = %d\n", my_uvc_params.bMinVersion);
}


static int my_uvc_vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
    int ret;
    printk("enter %s\n", __func__);

    /* 1. 向USB摄像头设置参数:比如使用哪个format, 使用这个format下的哪个frame(分辨率等) */
    // 根据结构体my_uvc_streaming_control设置数据包;再调用usb_control_msg发出数据包;

    //a.测试参数
    my_uvc_try_streaming_params(&my_uvc_params);
    //b.取出参数
    my_uvc_get_streaming_params(&my_uvc_params);
    //c.设置参数
    my_uvc_set_streaming_params(&my_uvc_params);

    //d.设置VideoStreaming Interface所使用的setting
    //从my_uvc_params.dwMaxPayloadTransferSize得知所需带宽;实测,分辨率不一样,所需的带宽也不一样;
    //根据wMaxPacketSize得到对应的bAlternateSetting;
    usb_set_interface(my_uvc_udev, my_uvc_streaming_intf, my_uvc_bAlternateSetting); 

    /* 2.分配设置URB */
    ret = my_uvc_alloc_init_urbs();
    if (0 != ret)
    {
        printk("my_uvc_alloc_init_urbs err : ret = %d\n", ret);
        return ret;
    }
        
    /* 3.提交URB以接收数据 */
    for (i = 0; i < MY_UVC_URBS_NUM; ++i)
    {
        if ((ret = usb_submit_urb(my_uvc_q.urb[i], GFP_KERNEL)) < 0)
        {
            printk("Failed to submit URB %u (%d).\n", i, ret);
            my_uvc_uninit_urbs();

            return ret;
        }
    }

    return 0;
}


static int my_uvc_vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type t)
{
    struct urb *urb;
    unsigned int i;

    printk("enter %s\n", __func__);

    /* 1. kill all URB */
    for (i = 0; i < MY_UVC_URBS_NUM; i++)
    {
        if ((urb = my_uvc_q.urb[i]) == NULL)
            continue;
        usb_kill_urb(urb);
    }

    /* 2. free all URB */
    my_uvc_uninit_urbs();

    /* 3. 设置VideoStreaming Interface为setting 0 */
    usb_set_interface(my_uvc_udev, my_uvc_streaming_intf, 0);
    /*这里就是probe()里为什么要两次打印才正确显示的bug,
    应用层在开始会在开始调用一次my_uvc_vidioc_streamoff,
    导致对应的接口变为0,因此第一次读取参数会失败*/

    return 0;
}

static const struct v4l2_ioctl_ops my_uvc_ioctl_ops =
{
    /* 表示它是一个摄像头设备 */
    .vidioc_querycap          = my_uvc_vidioc_querycap,
    /* 用于列举、获得、测试、设置摄像头的数据的格式 */
    .vidioc_enum_fmt_vid_cap  = my_uvc_vidioc_enum_fmt_vid_cap,
    .vidioc_g_fmt_vid_cap     = my_uvc_vidioc_g_fmt_vid_cap,
    .vidioc_try_fmt_vid_cap   = my_uvc_vidioc_try_fmt_vid_cap,
    .vidioc_s_fmt_vid_cap     = my_uvc_vidioc_s_fmt_vid_cap,
    /* 缓冲区操作: 申请/查询/放入队列/取出队列 */
    .vidioc_reqbufs           = my_uvc_vidioc_reqbufs,
    .vidioc_querybuf          = my_uvc_vidioc_querybuf,
    .vidioc_qbuf              = my_uvc_vidioc_qbuf,
    .vidioc_dqbuf             = my_uvc_vidioc_dqbuf,
    /* 查询/获取/设置属性(以亮度设置为例) */
    .vidioc_queryctrl         = my_uvc_vidioc_queryctrl,
    .vidioc_g_ctrl            = my_uvc_vidioc_g_ctrl,
    .vidioc_s_ctrl            = my_uvc_vidioc_s_ctrl,
    /* 启动/停止 */
    .vidioc_streamon          = my_uvc_vidioc_streamon,
    .vidioc_streamoff         = my_uvc_vidioc_streamoff,
};

static int my_uvc_close(struct file *file)
{
    printk("enter %s\n", __func__);

    my_uvc_vidioc_streamoff(NULL, NULL, 0);

    return 0;
}


static const struct v4l2_file_operations my_uvc_fops =
{
    .owner		    = THIS_MODULE,
    .open           = my_uvc_open,
    .release        = my_uvc_close,
    .mmap           = my_uvc_mmap,
    .unlocked_ioctl = video_ioctl2, /* V4L2 ioctl handler */
    .poll           = my_uvc_poll,
};

static struct video_device *my_uvc_vdev;
static struct v4l2_device v4l2_dev;

static void my_uvc_release(struct video_device *vdev)
{
    printk("enter %s\n", __func__);
}

static int my_uvc_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    static int cnt = 0;
    int ret;

    printk("enter %s\n", __func__);

    //usb_device_id会使probe()调用两次,然而创建video_device只需要一次
    cnt++;

    my_uvc_udev = interface_to_usbdev(intf); //获取usb设备
    if (cnt == 1) //获取编号
        my_uvc_control_intf = intf->cur_altsetting->desc.bInterfaceNumber;
    if (cnt == 2)
        my_uvc_streaming_intf = intf->cur_altsetting->desc.bInterfaceNumber;

    if (cnt == 2)
    {
        my_uvc_printk_streaming_params(); //为了确定带宽dwMaxVideoFrameSize,使用哪一个setting(可屏蔽)
        /* 需要放在video_register_device前,因为video_register_device后调用
        my_uvc_close-->my_uvc_vidioc_streamoff-->usb_set_interface,导致接口变成0,读出的值是空的*/
        
        /* 1.分配一个video_device结构体 */
        my_uvc_vdev = video_device_alloc();
        if (NULL == my_uvc_vdev)
        {
            printk("Faile to alloc video device (%d)\n", ret);
            return -ENOMEM;
        }

        /* 2.设置 */
        my_uvc_vdev->release   = my_uvc_release;
        my_uvc_vdev->fops      = &my_uvc_fops;
        my_uvc_vdev->ioctl_ops = &my_uvc_ioctl_ops;
        my_uvc_vdev->v4l2_dev  = &v4l2_dev;

        /* 3. 注册 */
        ret = video_register_device(my_uvc_vdev, VFL_TYPE_GRABBER, -1);
        if (ret < 0)
        {
            printk("Faile to video_register_device.\n");
            return ret;
        }
        else
            printk("video_register_device ok.\n");

    }
    return 0;
}

static void my_uvc_disconnect(struct usb_interface *intf)
{
    static int cnt = 0;

    printk("enter %s\n", __func__);

    cnt++;
    if (cnt == 2)
    {
        video_unregister_device(my_uvc_vdev);
        video_device_release(my_uvc_vdev);
    }
}

static struct usb_device_id my_uvc_ids[] =
{
    /* Generic USB Video Class */
    { USB_INTERFACE_INFO(USB_CLASS_VIDEO, 1, 0) },  /* VideoControl Interface */
    { USB_INTERFACE_INFO(USB_CLASS_VIDEO, 2, 0) },  /* VideoStreaming Interface */
    {}
};

//1.分配usb_driver
//2.设置
static struct usb_driver my_uvc_driver =
{
    .name       = "my_uvc",
    .probe      = my_uvc_probe,
    .disconnect = my_uvc_disconnect,
    .id_table   = my_uvc_ids,
};


static int my_uvc_init(void)
{
    //3.注册USB
    printk("enter %s\n", __func__);

    usb_register(&my_uvc_driver);
    return 0;
}

static void my_uvc_exit(void)
{
    printk("enter %s\n", __func__);

    usb_deregister(&my_uvc_driver);
}

module_init(my_uvc_init);
module_exit(my_uvc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng <huangcheng.job@foxmail.com>");
MODULE_DESCRIPTION("A USB Video Class driver For Learn.");
MODULE_ALIAS("My UVC");
MODULE_VERSION("V1.0");


