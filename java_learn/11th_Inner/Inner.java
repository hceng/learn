class OutClass {
    //一般内部类
    private int a = 1;
    class InnerClass1 {
        public void printInfo() {
            System.out.println("a = " + a);
        }
    }
    
    //静态内部类
    private static int b = 2;
    static class InnerClass2 {
        public void printInfo() {
            System.out.println("b = " + b);
        }
    }
}

interface PrintInterface {
    public void printInfo();
}

class MyInterface implements PrintInterface {
    public void printInfo() {
        System.out.println("MyInterface");
    }
}

public class Inner {
    public static void main(String args[]) {
        OutClass o = new OutClass(); //依次定义外部类
        OutClass.InnerClass1 i1 = o.new InnerClass1(); //内部类
        i1.printInfo();
        
        OutClass.InnerClass2 i2 = new OutClass.InnerClass2(); //直接定义内部类
        i2.printInfo();
        
        //正常调用接口类
        MyInterface m = new MyInterface();
        m.printInfo();
        
        //匿名内部类(没有名字的内部类,必须继承一个父类或实现一个接口)
        PrintInterface p = new PrintInterface() { 
            public void printInfo() {
                System.out.println("PrintInterface");
            }
        };
        p.printInfo();
    }
}

