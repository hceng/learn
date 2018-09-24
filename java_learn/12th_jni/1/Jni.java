public class Jni {
    /* 1.加载 */
    static { //静态代码块(只会被调用一次)
        System.loadLibrary("native"); //C语言会编译生成libnative.so,这里加载它
    }
    /* 2.声明 */
    public native static void hello_implicit(); //声明(static静态方法,可直接调用)
    public native static void hello_explicit(); 
    /* 3.调用 */
    public static void main(String args[]) {
        hello_implicit();
        hello_explicit();
    }
}