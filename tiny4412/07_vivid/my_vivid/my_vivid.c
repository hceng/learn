#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/font.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/videodev2.h>
#include <linux/v4l2-dv-timings.h>
#include <media/videobuf2-vmalloc.h>
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-dv-timings.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-event.h>
#include <media/v4l2-device.h>
#include <media/videobuf-core.h>
#include <media/videobuf-vmalloc.h>

#include "fillbuf.c"

/* 队列操作a: 定义 */
static struct videobuf_queue my_vivid_vb_vidqueue;
static spinlock_t my_vivid_queue_slock;
static struct list_head my_vivid_vb_local_queue;
static struct timer_list my_vivid_timer;

static void my_vivid_timer_function(unsigned long data)
{
    struct videobuf_buffer *vb;
    void *vbuf;
    struct timeval ts;

    printk("enter %s\n", __func__);

    /* 1. 构造数据: 从队列头部取出第1个videobuf, 填充数据 */

    /* 1.1 从本地队列取出第1个videobuf */
    if (list_empty(&my_vivid_vb_local_queue))
    {
        goto out;
    }

    vb = list_entry(my_vivid_vb_local_queue.next,
                    struct videobuf_buffer, queue);

    /* Nobody is waiting on this buffer, return */
    if (!waitqueue_active(&vb->done))
        goto out;


    /* 1.2 填充数据 */
    vbuf = videobuf_to_vmalloc(vb);
    //memset(vbuf, 0xFF, vb->size);

    my_vivid_fillbuff(vb);

    vb->field_count++;
    do_gettimeofday(&ts);
    vb->ts = ts;
    vb->state = VIDEOBUF_DONE;

    /* 1.3 把videobuf从本地队列中删除 */
    list_del(&vb->queue);

    /* 2. 唤醒进程: 唤醒videobuf->done上的进程 */
    wake_up(&vb->done);

out:
    /* 3. 修改timer的超时时间 : 30fps, 1秒里有30帧数据
     *    每1/30 秒产生一帧数据
     */
    mod_timer(&my_vivid_timer, jiffies + HZ / 30);
}


/* 参考documentations/video4linux/v4l2-framework.txt:
       drivers\media\video\videobuf-core.c
 ops->buf_setup   - calculates the size of the video buffers and avoid they to waste more than some maximum limit of RAM;
 ops->buf_prepare - fills the video buffer structs and calls videobuf_iolock() to alloc and prepare mmaped memory;
 ops->buf_queue   - advices the driver that another buffer were requested (by read() or by QBUF);
 ops->buf_release - frees any buffer that were allocated.
 */


/* videobuf operations */
//APP调用ioctl VIDIOC_REQBUFS时会导致此函数被调用,它重新调整count和size
static int my_vivid_buffer_setup(struct videobuf_queue *vq, unsigned int *count, unsigned int *size)
{
    printk("enter %s\n", __func__);

    *size = my_vivid_format.fmt.pix.sizeimage;

    if (0 == *count)
        *count = 32;

    return 0;
}

//APP调用ioctlVIDIOC_QBUF时导致此函数被调用,它会填充video_buffer结构体并调用videobuf_iolock来分配内存
static int my_vivid_buffer_prepare(struct videobuf_queue *vq, struct videobuf_buffer *vb,
                                   enum v4l2_field field)
{
    printk("enter %s\n", __func__);

    /* 1. 设置videobuf */
    vb->size         = my_vivid_format.fmt.pix.sizeimage;
    vb->bytesperline = my_vivid_format.fmt.pix.bytesperline;
    vb->width        = my_vivid_format.fmt.pix.width;
    vb->height       = my_vivid_format.fmt.pix.height;
    vb->field        = field;

    /* 2. 做些准备工作 */
    my_vivid_precalculate_bars(0);

    /* 3. 设置状态 */
    vb->state = VIDEOBUF_PREPARED;

    return 0;
}


/* APP调用ioctl VIDIOC_QBUF时:
 * 1. 先调用buf_prepare进行一些准备工作
 * 2. 把buf放入stream队列
 * 3. 调用buf_queue(起通知、记录作用)
 */
static void my_vivid_buffer_queue(struct videobuf_queue *vq, struct videobuf_buffer *vb)
{
    printk("enter %s\n", __func__);

    vb->state = VIDEOBUF_QUEUED;

    /* 把videobuf放入本地一个队列尾部
     * 定时器处理函数就可以从本地队列取出videobuf
     */
    list_add_tail(&vb->queue, &my_vivid_vb_local_queue);
}

/* APP不再使用队列时, 用它来释放内存 */
static void my_vivid_buffer_release(struct videobuf_queue *vq,
                                    struct videobuf_buffer *vb)
{
    printk("enter %s\n", __func__);

    videobuf_vmalloc_free(vb);

    vb->state = VIDEOBUF_NEEDS_INIT;
}

static struct videobuf_queue_ops my_vivid_video_qops =
{
    .buf_setup      = my_vivid_buffer_setup, /* 计算大小以免浪费 */
    .buf_prepare    = my_vivid_buffer_prepare,
    .buf_queue      = my_vivid_buffer_queue,
    .buf_release    = my_vivid_buffer_release,
};


/* v4l2_file_operations */
static int my_vivid_open(struct file *file)
{
    printk("enter %s\n", __func__);

    //队列操作c:初始化
    videobuf_queue_vmalloc_init(&my_vivid_vb_vidqueue, &my_vivid_video_qops,
                                NULL, &my_vivid_queue_slock, V4L2_BUF_TYPE_VIDEO_CAPTURE, V4L2_FIELD_INTERLACED,
                                sizeof(struct videobuf_buffer), NULL, NULL); /* 倒数第3个参数是buffer的头部大小 */

    my_vivid_timer.expires = jiffies + 1;
    add_timer(&my_vivid_timer);

    return 0;
}

static int my_vivid_close(struct file *file)
{
    printk("enter %s\n", __func__);

    del_timer(&my_vivid_timer);
    videobuf_stop(&my_vivid_vb_vidqueue);
    videobuf_mmap_free(&my_vivid_vb_vidqueue);

    return 0;
}

static int my_vivid_mmap(struct file *file, struct vm_area_struct *vma)
{
    printk("enter %s\n", __func__);

    return videobuf_mmap_mapper(&my_vivid_vb_vidqueue, vma);
}

static unsigned int my_vivid_poll(struct file *file, struct poll_table_struct *wait)
{
    printk("enter %s\n", __func__);

    return videobuf_poll_stream(file, &my_vivid_vb_vidqueue, wait);
}


/* v4l2_ioctl_ops */
static int my_vivid_vidioc_querycap(struct file *file, void  *priv,
                                    struct v4l2_capability *cap)
{
    printk("enter %s\n", __func__);

    strcpy(cap->driver, "my_vivid");
    strcpy(cap->card, "my_vivid");
    cap->version = 0x0001;

    cap->device_caps  = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS;
    cap->capabilities =	V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS;

    return 0;
}

static int my_vivid_vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
        struct v4l2_fmtdesc *f)
{
    printk("enter %s\n", __func__);

    if (f->index >= 1)
        return -EINVAL;

    strcpy(f->description, "4:2:2, packed, YUYV");
    f->pixelformat = V4L2_PIX_FMT_YUYV;

    return 0;
}

static int my_vivid_vidioc_g_fmt_vid_cap(struct file *file, void *priv,
        struct v4l2_format *f)
{
    printk("enter %s\n", __func__);

    memcpy(f, &my_vivid_format, sizeof(my_vivid_format));

    return 0;
}

static int my_vivid_vidioc_try_fmt_vid_cap(struct file *file, void *priv,
        struct v4l2_format *f)
{
    unsigned int maxw, maxh;
    enum v4l2_field field;

    printk("enter %s\n", __func__);

    if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV)
        return -EINVAL;

    field = f->fmt.pix.field;

    if (field == V4L2_FIELD_ANY)
    {
        field = V4L2_FIELD_INTERLACED;
    }
    else if (V4L2_FIELD_INTERLACED != field)
    {
        return -EINVAL;
    }

    maxw  = 1024;
    maxh  = 768;

    /* 调整format的width, height,
     * 计算bytesperline, sizeimage
     */
    v4l_bound_align_image(&f->fmt.pix.width, 48, maxw, 2,
                          &f->fmt.pix.height, 32, maxh, 0, 0);
    f->fmt.pix.bytesperline =
        (f->fmt.pix.width * 16) >> 3;
    f->fmt.pix.sizeimage =
        f->fmt.pix.height * f->fmt.pix.bytesperline;

    return 0;
}

static int my_vivid_vidioc_s_fmt_vid_cap(struct file *file, void *priv,
        struct v4l2_format *f)
{
    int ret;

    printk("enter %s\n", __func__);

    ret = my_vivid_vidioc_try_fmt_vid_cap(file, NULL, f);

    if (ret < 0)
        return ret;

    memcpy(&my_vivid_format, f, sizeof(my_vivid_format));

    return ret;
}

static int my_vivid_vidioc_reqbufs(struct file *file, void *priv,
                                   struct v4l2_requestbuffers *p)
{
    printk("enter %s\n", __func__);

    return (videobuf_reqbufs(&my_vivid_vb_vidqueue, p));
}

static int my_vivid_vidioc_querybuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
    printk("enter %s\n", __func__);

    return (videobuf_querybuf(&my_vivid_vb_vidqueue, p));
}

static int my_vivid_vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
    printk("enter %s\n", __func__);

    return (videobuf_qbuf(&my_vivid_vb_vidqueue, p));
}

static int my_vivid_vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *p)
{
    printk("enter %s\n", __func__);

    return (videobuf_dqbuf(&my_vivid_vb_vidqueue, p, file->f_flags & O_NONBLOCK));
}

static int my_vivid_vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
    printk("enter %s\n", __func__);

    return videobuf_streamon(&my_vivid_vb_vidqueue);
}

static int my_vivid_vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type i)
{
    printk("enter %s\n", __func__);

    videobuf_streamoff(&my_vivid_vb_vidqueue);

    return 0;
}


static const struct v4l2_ioctl_ops my_vivid_ioctl_ops =
{
    // 表示它是一个摄像头设备
    .vidioc_querycap          = my_vivid_vidioc_querycap,

    /* 用于列举、获得、测试、设置摄像头的数据的格式 */
    .vidioc_enum_fmt_vid_cap  = my_vivid_vidioc_enum_fmt_vid_cap,
    .vidioc_g_fmt_vid_cap     = my_vivid_vidioc_g_fmt_vid_cap,
    .vidioc_try_fmt_vid_cap   = my_vivid_vidioc_try_fmt_vid_cap,
    .vidioc_s_fmt_vid_cap     = my_vivid_vidioc_s_fmt_vid_cap,

    /* 缓冲区操作: 申请/查询/放入队列/取出队列 */
    .vidioc_reqbufs       = my_vivid_vidioc_reqbufs,
    .vidioc_querybuf      = my_vivid_vidioc_querybuf,
    .vidioc_qbuf          = my_vivid_vidioc_qbuf,
    .vidioc_dqbuf         = my_vivid_vidioc_dqbuf,

    // 启动/停止
    .vidioc_streamon      = my_vivid_vidioc_streamon,
    .vidioc_streamoff     = my_vivid_vidioc_streamoff,
};


static const struct v4l2_file_operations my_vivid_fops =
{
    .owner		    = THIS_MODULE,
    .open           = my_vivid_open,
    .release        = my_vivid_close,
    .mmap           = my_vivid_mmap,
    .unlocked_ioctl = video_ioctl2, /* V4L2 ioctl handler */
    .poll           = my_vivid_poll,
};

static struct video_device *my_vivid_dev;
static struct v4l2_device v4l2_dev;


static void my_vivid_dev_release(struct video_device *vdev)
{
    printk("enter %s\n", __func__);
}


static int my_vivid_probe(struct platform_device *pdev)
{
    int ret;
    printk("enter %s\n", __func__);

    /* 1.分配一个video_device结构体 */
    my_vivid_dev = video_device_alloc();
    if (NULL == my_vivid_dev)
    {
        printk("Failed to alloc video device (%d)\n", ret);
        return -ENOMEM;
    }

    /* 2.设置 */
    my_vivid_dev->release   = my_vivid_dev_release;
    my_vivid_dev->fops      = &my_vivid_fops;
    my_vivid_dev->ioctl_ops = &my_vivid_ioctl_ops;
    my_vivid_dev->v4l2_dev  = &v4l2_dev;

    //队列操作b://定义/初始化一个队列
    spin_lock_init(&my_vivid_queue_slock);

    /* 3.注册 */
    ret = video_register_device(my_vivid_dev, VFL_TYPE_GRABBER, -1);
    if (ret)
    {
        printk("Failed to register as video device (%d)\n", ret);
        goto err_register_dev;
    }

    //用定时器产生数据并唤醒进程
    init_timer(&my_vivid_timer);
    my_vivid_timer.function  = my_vivid_timer_function;
    INIT_LIST_HEAD(&my_vivid_vb_local_queue);

    return 0;

err_register_dev:
    video_device_release(my_vivid_dev);
    return -ENODEV;
}

static int my_vivid_remove(struct platform_device *pdev)
{
    printk("enter %s\n", __func__);

    v4l2_device_unregister(my_vivid_dev->v4l2_dev);
    video_device_release(my_vivid_dev);

    return 0;
}

static void my_vivid_pdev_release(struct device *dev)
{
    printk("enter %s\n", __func__);
}

static struct platform_device my_vivid_pdev =
{
    .name		 = "my_vivid",
    .dev.release = my_vivid_pdev_release,
};

static struct platform_driver my_vivid_pdrv =
{
    .probe		= my_vivid_probe,
    .remove		= my_vivid_remove,
    .driver		= {
        .name	= "my_vivid",
    },
};

static int my_vivid_init(void)
{
    int ret;

    printk("enter %s\n", __func__);

    ret = platform_device_register(&my_vivid_pdev);
    if (ret)
        return ret;

    ret = platform_driver_register(&my_vivid_pdrv);
    if (ret)
        platform_device_unregister(&my_vivid_pdev);

    return ret;

}

static void my_vivid_exit(void)
{
    printk("enter %s\n", __func__);

    platform_driver_unregister(&my_vivid_pdrv);
    platform_device_unregister(&my_vivid_pdev);
}

module_init(my_vivid_init);
module_exit(my_vivid_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hceng <huangcheng.job@foxmail.com>");
MODULE_DESCRIPTION("A Virtual Video Test Code For Learn.");
MODULE_ALIAS("My vivid");
MODULE_VERSION("V1.0");

