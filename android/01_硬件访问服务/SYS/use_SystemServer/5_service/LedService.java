package com.android.server;
import android.os.ILedService;

public class LedService extends ILedService.Stub {
	private static final String TAG = "ILedService";
	
	//Call native c function to access hardware
	public int ledCtrl(int number, int status) throws android.os.RemoteException 
	{
		return native_ledCtrl(number, status);
	}
	
	public LedService() {
		native_ledOpen();
	}
	
	//Function declaration
	public static native int native_ledCtrl(int number, int status);
	public static native int native_ledOpen();
	public static native void native_ledClose();
}