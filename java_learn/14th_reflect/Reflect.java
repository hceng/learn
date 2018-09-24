package hceng;
import java.lang.reflect.Method;
import java.lang.reflect.Field;
import java.lang.reflect.Constructor;

class Person {
    private String name;
    
    void setName(String name) {
        this.name = name;
    }
    
    String getName() {
        return this.name;
    }
};

public class Reflect {
    public static void main(String args[]) throws Exception {
        
		//获得class的方法一
        Class<?> c1 = Class.forName("hceng.Person");  //must be caught or declared to be thrown

		//获得class的方法二
        Person p = new Person();
        Class<?> c2 = p.getClass();

		//获得class的方法三
        Class<?> c3 = Person.class;
        
        System.out.println(c1.getName());
        System.out.println(c2.getName());
        System.out.println(c3.getName());
        
        System.out.println("--------------------------");       
 
        int arr1[] = {1, 2, 3};
        int arr2[][] = {{1, 2, 3}, {1,2}};
        
        Class<?> a1 = arr1.getClass();
        Class<?> a2 = arr2.getClass();
        Class<?> a3 = int.class;
        
        System.out.println(a1.getName());
        System.out.println(a2.getName());
        System.out.println(a3.getName());
        
        System.out.println("--------------------------");   
		//类的实例化
        Class<?> j = Class.forName("jack.Person"); 
        
        Object p1 = j.newInstance(); //无参构造方法
        
        Constructor<?> con = j.getConstructor(String.class); //有参构造方法
        Object p2 =  con.newInstance("hceng");
        
        System.out.println("--------------------------");   
        
        //通过反射调用方法
        Method set =  j.getMethod("setName", String.class);
        set.invoke(p1, "jack1");
        set.invoke(p2, "jack2");

        Method get = j.getMethod("getName");
        System.out.println(get.invoke(p1));
        System.out.println(get.invoke(p2)); 
        
        System.out.println("--------------------------");   
        
        //通过修改属性
        Field name = j.getDeclaredField("name"); //公共、私有的都可以访问
        name.setAccessible(true); //因为name是私有的,需要先修改访问权限
        name.set(p1, "hello, hceng");
        name.set(p2, "hello, jack");
        System.out.println(name.get(p1));
        System.out.println(name.get(p2));
        
        System.out.println("--------------------------");   
        //通过传入参数,灵活调用不同的包
        Class<?> h = Class.forName(args[0]); 
        Constructor<?> c = h.getConstructor(String.class);
        Object o =  c.newInstance("hceng");
    }
}