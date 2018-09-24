package pack.hceng; 

//一个文件中只能有一个public类,从而被外部访问
public class Permission { 

	static private int a = 1; //通过static修饰的变量叫类变量，可以不通过new创建对象进行访问
	static int b = 2;
	static protected int c = 3;
	static public int d = 4;
	
	public static void printInfo() { //通过static修饰的方法叫类方法，可以不通过new创建对象进行访问
		System.out.println("Can only be accessed by this class: a = " + a);
		packagePermission.printInfo();	
	}
}

//没有public的类,只能在本包访问
class packagePermission {
	public static void printInfo() {
		System.out.println("Access in the same package: b = " + Permission.b);
	}
}


