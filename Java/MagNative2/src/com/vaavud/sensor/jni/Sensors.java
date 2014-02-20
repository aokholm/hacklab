package com.vaavud.sensor.jni;

public class Sensors {
	

	public static native String getSensorName();
	public static native void startSensors();
	public static native void stopSensors();
	public static native void setString(String str);
	public static native void setListener(SensorsListener listener);
	
	static {
        System.loadLibrary("sensors");
    }
	
}
