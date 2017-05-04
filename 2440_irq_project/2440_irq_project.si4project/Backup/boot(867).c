
extern void uart0_init(void);
extern void puts(char *str);
extern void puthex(unsigned int val);

int strlen(char *str)
{
	int i = 0;
	while (str[i])
	{
		i++;
	}
	return i;
}

void strcpy(char *dest, char *src)
{
	while ((*dest++ = *src++) != '\0');
}

int main(void)
{
	/* 0. 帮内核设置串口: 内核启动的开始部分会从串口打印一些信息,但是内核一开始没有初始化串口 */
	uart0_init();
	while(1)
	{
	/* 1. 从NAND FLASH里把内核读入内存 */
		puts("ok.\n\r");
	}
	return -1;
}

