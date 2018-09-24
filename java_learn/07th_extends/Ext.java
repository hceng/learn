class Person {
    //变量/属性
    private int age; 
    
    //方法
    public void setAge(int age) {
        if (age < 0 || age > 200)
            age = 0;
        else 
            this.age = age;
    }
    
    public int getAge() {
        return age;
    }
    
    public void printfInfo() {
        System.out.println("age = " + age);
    }
    
    final public void testFinal() {
        
    }
    
    //构造方法
    public Person() { //系统默认的构造函数,如果没有,编译器会自动生成
        System.out.println("for test Person() call");
    }
    
    public Person(int age) {
        this.age = age;
        System.out.println("for test Person(int age) call");
    }
}

//Student从父类(Person)继承
class Student extends Person {
    private String school; 
    
    public void setSchool(String school) {
        this.school = school; 
    }
    
    public String getSchool() {
        return this.school;
    }
    
    //覆写(方法名在父类中有同名方法)
    public void printfInfo() {
        System.out.println("school = " + school);
    }
    
    //父类方法用了final修饰,子类无法覆写
    //public void testFinal() {
    //  
    //}
    
    //构造方法
    public Student() { //系统默认的构造函数,如果没有,编译器会自动生成
        //super(); //调用父类的构造函数(无参数),默认调用,可不写
        super(15); //调用父类的构造函数(有参数)
        super.printfInfo(); //super就指代的父类
        System.out.println("for test Student() call");
    }
}

public class Ext {
    public static void main(String args[]) {
        Student per = new Student();
        
        per.setAge(10); //方法来源于父类
        System.out.println(per.getAge());
        
        per.setSchool("ShenZhen"); //方法来源于自己扩展
        System.out.println(per.getSchool());
        
        per.printfInfo(); //覆写的父类方法
    }
}