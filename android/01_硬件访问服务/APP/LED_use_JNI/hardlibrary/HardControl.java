package cn.hceng.hardlibrary;

public class HardControl {
    //1.load
    static {
        try {
            System.loadLibrary("hardcontrol"); //Call libhardcontrol.so from C.
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    //2.Function declaration
    public static native int ledCtrl(int number, int status);
    public static native int ledOpen();
    public static native void ledClose();
}