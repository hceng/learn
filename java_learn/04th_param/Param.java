public class Param {
	public static void main(String args[]) {
		int x = 1;
		System.out.println("Before x=" + x);
		fun(x);
		System.out.println("After  x=" + x);
		
		int p[] = new int[1];
		p[0] = 2;
		System.out.println("Before p=" + p[0]);
		fun(p);
		System.out.println("After  p=" + p[0]);
		
	}
	
	public static void fun(int x) {
		x = 100;
	}
	
	public static void fun(int[] p) {
		p[0] = 200;
	}
}