class Father {
    private int money;
        
    public void setMoney(int money) {
        this.money = money;
    }
    
    public void printfInfo1() {
        System.out.println("This is Father printfInfo1()");
    }
    
    private void printfInfo2() {
        System.out.println("This is Father printfInfo2()");
    }
}

class Son extends Father {
    //private void printfInfo1() {
        //System.out.println("This is Son printfInfo1()");
    //}
    
    public void printfInfo2() {
        System.out.println("This is Son printfInfo2()");
    }
}

public class Limit {
    public static void main(String args[]) {
        Son son = new Son();
        
        //son.money = 100;
        son.setMoney(100);
        
        son.printfInfo1();
        son.printfInfo2();
    }
}