package com.vaavud.serial;

import com.vaavud.sensor.BaseSensor;
import com.vaavud.sensor.SensorListener;
import com.vaavud.serial.internal.SerialPortController;

public class SerialSensor extends BaseSensor {
	
	private SerialPortController serialPortController;
	
	public SerialSensor() {
		serialPortController = SerialPortController.getInstance();
	}
	
	@Override
	public void start() throws Exception {
		serialPortController.start();
	}
	
	@Override
	public void stop() {
		SerialPortController.closeConnection();

	}

	@Override
	public void setReciever(SensorListener listener) {
		serialPortController.setListener(listener);
	}

}
