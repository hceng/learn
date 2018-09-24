//普通类
class Person1 {
    private int age;
    
    public void setAge(int age) {
        this.age = age;
    }
    
    public int getAge() {
        return this.age;
    }
}

//泛型
class Person2<T> {
    private T age;
    
    public void setAge(T age) {
        this.age = age;
    }
    
    public T getAge() {
        return this.age;
    }
}

//泛型接口
interface Person3<T> {
    public void setAge(T age);
    public T getAge();
}


//一般继承
class Student1<T> extends Person2<T> {

}

class Student2 extends Person2<String> {
    
}

//接口继承
class Student3<T> implements Person3<T> {
    T age;
    
    public void setAge(T age) {
        this.age = age;
    }
    
    public T getAge() {
        return this.age;
    } 
}

class Student4 implements Person3<String> {
    String age;
    
    public void setAge(String age) {
        this.age = age;
    }
    
    public String getAge() {
        return this.age;
    } 
}


public class Generics {
    public static void main(String args[]) {
        //常规方法,传入参数数据类型固定
        Person1 p1 = new Person1();
        p1.setAge(23);
        System.out.println(p1.getAge());
        System.out.println("--------------------------");
        
        //泛型,传入参数数据类型可以不固定
        Person2<Integer> p2 = new Person2<Integer>();
        p2.setAge(24);
        printInfo(p2);
        genericsMethod(p2);
        
        Person2<String> p3 = new Person2<String>();
        p3.setAge("24 years old");
        printInfo(p3);
        genericsMethod(p3);
        System.out.println("--------------------------");
        
        //定义数据类型通用的对象
        Person2<?> p4;
        p4 = p2; 
        printInfo(p4); //无法 p4.setAge();
        p4 = p3; 
        printInfo(p4);
        System.out.println("--------------------------");
        
        //子类继承:子类也泛型
        Student1<Integer> s1 = new Student1<Integer>();
        s1.setAge(10);
        printInfo(s1);
        genericsMethod(s1);
        
        //子类继承:子类不泛型
        Student2 s2 = new Student2();
        s2.setAge("10 years old");
        printInfo(s2);
        genericsMethod(s2);
        System.out.println("--------------------------");
        
        //接口子类继承:子类也泛型
        Student3<Integer> s3 = new Student3<Integer>();
        s3.setAge(10);
        System.out.println("interface:" + s3.getAge());
        
        //子类继承:子类不泛型
        Student4 s4 = new Student4();
        s4.setAge("10 years old");
        System.out.println("interface:" + s4.getAge());
        System.out.println("--------------------------");
        
        //受限泛型
        printInfo1(s1); //参数限制了只能是Number类或其子类
        //printInfo1(s2);
        //printInfo2(s1);
        printInfo2(s2); //参数限制了只能是String类或其父类
    }
    
    //方法参数通用
    public static void printInfo(Person2<?> p) {
        System.out.println("printInfo:" + p.getAge());
    }
    
    //方法的参数泛型
    public static <T> void genericsMethod(Person2<T> p) {
        System.out.println("genericsMethod:" + p.getAge());
    }
    
    //受限泛型
    //上限:参数只能是Number类或其子类
    public static void printInfo1(Person2<? extends Number> p) {
        System.out.println("extends:" + p.getAge());
    }
    //下限:参数只能是String类或其父类
    public static void printInfo2(Person2<? super String> p) {
        System.out.println("extends:" + p.getAge());
    }
}
