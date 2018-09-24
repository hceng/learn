class Father {
    public void printfInfo() {
        System.out.println("This is Father");
    }

    public void work() {
        System.out.println("Father is working……");
    } 
}

class Son extends Father {
    public void printfInfo() {
        System.out.println("This is Son");
    } 
    
    public void palyGame() {
        System.out.println("Son is playing games……");
    } 
}

class Daughter extends Father {
    public void printfInfo() {
        System.out.println("This is Daughter");
    } 
    
    public void dance() {
        System.out.println("Daughter is dancing……");
    } 
}

public class Cnv {
    public static void main(String args[]) {
        Father father1 = new Father();
        Son son = new Son();
        father1 = son; //向上转换
        father1.printfInfo();
        //father1.palyGame(); //只能调用被子类覆写的方法，不能调用只在子类中定义的方法
        
        Father father2  = new Daughter(); //创建子类的实例化对象(先向上转换)
        Daughter daughter = (Daughter)father2; //向下转换 (Daughter daughter = new Daughter();)
        daughter.printfInfo();  
        
        System.out.println("--------------------");
        
        Father f = new Father();
        Son s = new Son();
        Daughter d = new Daughter();
        //向上转换 示例
        printf(f);
        printf(s);
        printf(d);      
        //向下转换 示例       
        printAction(f);
        printAction(s);
        printAction(d); 
    } 
    
    public static void printf(Father f) {
        f.printfInfo();
    }
    
    public static void printAction(Father f) {
        if (f instanceof Son) {
            Son s = (Son)f;
            s.palyGame();
        }
        else if (f instanceof Daughter) {
            Daughter d = (Daughter)f;
            d.dance();
        }
        else if (f instanceof Father) { //Father要在最后,每个对象都属于Father
            f.work();
        }
    }
}