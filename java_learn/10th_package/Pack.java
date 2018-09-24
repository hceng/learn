import pack.jack.*;
import pack.hceng.*;

public class Pack {
	public static void main(String args[]) {
		System.out.println("add1:"+pack.jack.Math.add(1, 2));
		System.out.println("add2:"+pack.hceng.Math.add(1, 2));
		
		System.out.println("add2:"+pack.hceng.Math.sub(1, 2));
		
		pack.hceng.Permission.printInfo(); //Permission.printInfo();
		pack.jack.TestAccess.printInfo();
		System.out.println("Accessible anywhere: d = " + Permission.d);
		
	}
}