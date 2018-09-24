public class Hello {
    private static String name;
    private static int age;
    
    public static void main(String args[]) { //静态方法
        System.out.println("java: name: "+ name + ", age: " + age);
    }
    
    public int typeData(String str) { //非静态方法
        System.out.println("java: get str: "+ str + " ,will return 100");
        return 100;
    }
}