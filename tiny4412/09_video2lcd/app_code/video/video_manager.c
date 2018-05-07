
#include <string.h>
#include <config.h>
#include <video_manager.h>

//构造一个操作函数的链表
static p_video_operations p_video_ops_head = NULL;

//加入摄像头操作函数链表
int register_video_ops(p_video_operations p_video_ops)
{
    //为了保持p_video_ops_head始终指向链表头,定义一个p_tmp来移动到链表尾
    p_video_operations p_tmp;

    if (NULL == p_video_ops_head)
    {
        p_video_ops_head    = p_video_ops;
        p_video_ops->p_next = NULL;
    }
    else
    {
        p_tmp = p_video_ops_head;
        while (p_tmp->p_next)
        {
            p_tmp = p_tmp->p_next;
        }
        p_tmp->p_next       = p_video_ops;
        p_video_ops->p_next = NULL;
    }

    return 0;
}

//显示链表中的每个操作函数名字
void show_video_ops(void)
{
    int i = 0;
    p_video_operations p_tmp = p_video_ops_head;

    printf("video operations lists:\n");

    while (p_tmp)
    {
        printf("%02d %s\n", i++, p_tmp->name);
        p_tmp = p_tmp->p_next;
    }
}

//根据名字从链表获取对应操作函数结构体
p_video_operations get_video_ops(char *name)
{
    p_video_operations p_tmp = p_video_ops_head;

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

//遍历链表,初始化指定名字(/dev/fb0)的设备
int video_device_init(char *dev_name, p_video_device p_video_dev)
{
    int ret;
    p_video_operations p_tmp = p_video_ops_head;

    while (p_tmp)
    {
        ret = p_tmp->init_device(dev_name, p_video_dev);
        if (!ret)
            return 0;

        p_tmp = p_tmp->p_next;
    }
    return -1;
}

int video_init(void)
{
    int ret;

    ret = v4l2_init();

    return ret;
}


