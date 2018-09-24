//接口
interface A {
    public static final int i = 10; //接口只能定义常量
    public abstract void printNum();
}

interface B {
    public static final String name = "hceng"; 
    public abstract void printString();
}

//子类
class Son implements A,B {
    public void printNum() { //实现接口定义的方法
        System.out.println("Num = " + i); 
    }
    
    public void printString() { 
        System.out.println("String = " + name); 
    }
}

public class Interface {
    public static void main(String args[]) {
       
        Son son = new Son();
        
        son.printNum();
        son.printString();
    }
}