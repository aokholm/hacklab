package com.vaavud.serial;

import com.vaavud.ars.BaseSensor;
import com.vaavud.ars.SensorListener;
import com.vaavud.serial.internal.SerialPortController;

public class SerialSensor extends BaseSensor {
	
	private SerialPortController serialPortController;
	
	public SerialSensor() {
		serialPortController = SerialPortController.getInstance();
	}
	
	@Override
	public void start() {
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
