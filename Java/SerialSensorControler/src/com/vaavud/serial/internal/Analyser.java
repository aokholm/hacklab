package com.vaavud.serial.internal;

import com.vaavud.sensor.SensorEvent;
import com.vaavud.sensor.SensorType;

public class Analyser {
	
	private static StringBuffer stringBuffer;
	
	public Analyser() {
		stringBuffer = new StringBuffer();
	}
	
	public void append(String serialString) {
		stringBuffer.append(serialString);
	}
	
	public SensorEvent readMeasurement() throws Exception {
		
		// if no complete line return null 
		if (stringBuffer.indexOf("\n") == -1) {
			return null;
		}
		
		// if no decimal, clear string and return null 
		if (stringBuffer.indexOf(",") == -1){
			stringBuffer.delete(0, stringBuffer.indexOf("\n"));
			return null;
			
		}
		
		long timeUs = getNextValue(",");
		double[] values = new double[] {getNextValue(","), getNextValue(","), getNextValue("\n")};
		SensorEvent event = new SensorEvent(SensorType.TYPE_MAGNETIC_FIELD, timeUs, values);
		
		return event;
	}
	
	private Integer getNextValue(String seperator) {
		
		int	sepIndex = stringBuffer.indexOf(seperator);
		if (sepIndex != -1) {
			int val = Integer.parseInt(stringBuffer.substring(0,sepIndex));
			stringBuffer.delete(0, sepIndex+1);
			return val;
		} else {
			return null;
		}
	}
 
	
}
