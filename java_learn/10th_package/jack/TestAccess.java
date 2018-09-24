package pack.jack;
import pack.hceng.Permission;

public class TestAccess extends Permission {
	public static void printInfo() {
		System.out.println("Access in different packages: c = " + Permission.c);
	}
}
