package jack;

public class Person {
    private String name;
    
    public void setName(String name) {
        this.name = name;
    }
    
    public String getName() {
        return this.name;
    }
    
    //构造方法
    public Person() {
        System.out.println("Constructor1 of Person");
    }
    
    public Person(String name) {
        this.name = name;
        System.out.println("Constructor2 of Person, name is "+this.name);
    }
};
