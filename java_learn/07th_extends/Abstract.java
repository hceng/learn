//抽象类
abstract class Father {
    public abstract void study(); //规定了子类必须实现的方法
}

//子类
class Son extends Father {
    public void study() { //实现抽象类定义的方法
        System.out.println("Son study"); 
    }
}

public class Abstract {
    public static void main(String args[]) {
        //Father father = new Father(); //抽象类不能实例化对象
        Son son = new Son();
        
        son.study();
    }
}