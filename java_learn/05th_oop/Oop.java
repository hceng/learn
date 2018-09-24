//类的定义(类是创建对象的模板,创建对象也叫类的实例化)
class Person {
	
	//类变量(调用不需要实例化对象)
	static int count; 
	
	//类方法(调用不需要实例化对象)
	static void printPerson () {
		System.out.println("This is a class of Person");
	} 
	
	//静态代码块(只会被调用一次,且先于构造代码块、构造方法执行)
	static {
		System.out.println("Execute only once");
	}
	
	//构造代码块(每次创建对象时都会被调用,且先于构造方法执行)	
	{
		System.out.println("Every call is executed");
		count ++;
	}
	
	//类的成员
	String name; //成员变量
	int age;     //成员变量
	//成员方法
	String getName() { 
		return name;
	}

	
	//构造方法(没有返回值，方法名必须和类名一样)
	//实现new时传入参数,重载实现传入的参数多样化
	public Person () {
		name = "null";
		age  = 0;
	}
	
	public Person (String name) {
		this.name = name; //name是局部变量
		age  = 0;
	}
	
	public Person (String name, int age) {
		this.name = name;
		this.age  = age;
	}
}

//一个源文件中只能有一个public类,且与文件名相同
public class Oop {
	public static void main(String args[]) {
		Person p0 = new Person();         //创建对象,即类的实例化
		Person p1 = new Person("hceng");
		Person p2 = new Person("hceng, 23");
		
		System.out.println(p0.getName()); //调用成员方法
		System.out.println(p1.getName());
		System.out.println(p2.getName());
		
		Person.printPerson();             //访问类方法
		System.out.println("Person number is " + Person.count); //访问类变量
	}
}