public class Var {
    public static void main(String args[]) {
    //基本数据类型:变量名指向具体的数值(数据在栈)
        boolean a = true;  //布尔型
        byte    b = 1;     //字节型
        char    c = 'h';   //字符型
        short   d = 2;     //短整数
        int     e = 3;     //整型
        long    f = 4L;    //长整型
        float   g = 3.14F; //单精度浮点型
        double  h = 3.14D; //双精度浮点型
        
    //引用数据类型:变量名指向存数据对象的内存地址(引用在栈,数据在堆)
        int p1[] = new int[10]; //分配整数数组,类似C语言int p1[10];或int* p1 = malloc(10*sizeof(int));
        int p2[] = {1, 2, 3};   //分配并定义,类似C语言int p2[10] = {1, 2, 3};
                
        String str = "hceng";  //类引用,分配并定义,类似C语言char str[] = "hceng";
        
        p1  = null; //自动释放
        p2  = null;
        str = null;
        
    //数据转换
        int  a1 = 30;
        long b1 = a1;       //自动转换,因为long的表示范围比int大
        float c1 = 3.1f;
        int   d1 = (int)c1; //必须使用强制转换,因为数据会丢失
        
        short s = 1;
        //s = s + 1;  //出错,因为s+1自动将s变成了int型与1相加,再赋值给short型的s,大范围变小范围不行
        //s = s + s;  //出错,因为s+s也会自动变成int型赋值给short型的s,大范围变小范围不行
        s = (short)(s + 1);
        s = (short)(s + s);
    }
}



