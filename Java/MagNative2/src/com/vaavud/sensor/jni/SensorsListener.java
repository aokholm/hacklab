package com.vaavud.sensor.jni;

public interface SensorsListener {
	public void onReturnedString(String str);
    
    public void onReturnedSensorValue(long timestamp, float x, float y, float z);
}
