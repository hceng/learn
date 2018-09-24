public class Jni {

    static { 
        System.loadLibrary("native"); 
    }
    
    //基本类型数据
    public native static float typeData1(int a); 
    
    //字符串 
    public native static String typeData2(String str); 
    
    //数组
    public native static int[] typeData3(int[] a); 

    public static void main(String args[]) {
        int [] a = {1, 2, 3, 4}; 
        int [] b = null;
        int i;
        
        System.out.println(typeData1(4));
        System.out.println(typeData2("hceng"));
        
        b = typeData3(a);
        for (i = 0; i < b.length; i++)      
            System.out.print(b[i]+" ");
        System.out.println();
    }
}