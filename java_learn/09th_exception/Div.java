
/* 除法运算: java Div 6 2   ->  6/2=3*/

public class Div {
    public static void main(String args[]) {
        int m, n, r;
        m=n=r=0; //初始化

        try {
            m = Integer.parseInt(args[0]);
            n = Integer.parseInt(args[1]);
            r = div(m, n);
            craetException();
        } catch (NumberFormatException exception1) { //自己处理:数字格式异常(java Div 6 a)
            System.out.println("====An exception occurs1:"+exception1+"=====");
        } catch (ArithmeticException exception2) { //处理扔过来的异常:除零错误异常(java Div 6 0)
            System.out.println("====An exception occurs2:"+exception2+"=====");
        } catch (RuntimeException runtimeexception) { //处理所有不可查异常,比如位数不够(java Div 6)
            System.out.println("====An exception occurs:"+runtimeexception+"=====");
        } catch (Exception exception) { //处理自己通过throw创建的可查异常
            System.out.println("====An exception occurs:"+exception+"=====");
        } 
        finally {
            //无论是否发生异常,finally代码块中的代码总会被执行,完成清理类型等收尾善后性质的功能
            System.out.println("finally"); //finally中不要包含return
        }
        
        System.out.println(m+"/"+n+"="+r);
    }
    
    //扔给调用者处理:除零错误异常(java Div 6 0)
    //public static int div(int m, int n) throws ArithmeticException {
    public static int div(int m, int n) {  //ArithmeticException是不可查异常,出现后会自动抛出,可以不要throws
        int r;
        r = m / n;
        return r;
    }
    //自己创建的可查异常NullPointerException,必须处理,这里将其抛出,因此必须要throws
    public static void craetException() throws Exception {
        throw new Exception("My Craeted Exception");
    }
}