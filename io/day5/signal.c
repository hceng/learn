#include <signal.h>
#include <stdio.h>
//signo代表内核相当前进程发送的信号时什么？而我们又不清楚时那个信号
//需要用if语句做判断
void fun(int signo)
{
	if(signo == SIGINT)
	{
		printf("hello world \n");
	}
}

int main(int argc, const char *argv[])
{
	//信号处理函数，本身不具有阻塞作用
	//第一个参数： 要出里的信号是什么？62中的一种
	//第二个参数： 三种相应方式：
	//忽略（SIG_IGN）、默认(SIG_DFL)、捕捉(用户自定义函数)
	signal(SIGINT, fun);
	pause();
	return 0;
}
