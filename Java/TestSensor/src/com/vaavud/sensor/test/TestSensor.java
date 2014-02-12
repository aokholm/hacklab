package com.vaavud.sensor.test;

import com.vaavud.sensor.BaseSensor;
import com.vaavud.sensor.SensorListener;
import com.vaavud.sensor.test.internal.EventGenerator;

public class TestSensor extends BaseSensor{
    
    private EventGenerator eventGenerator;
    
    public TestSensor() {
        eventGenerator = new EventGenerator();
    }
    
    @Override
    public void start() throws Exception {
        eventGenerator.start();
    }
    
    @Override
    public void stop() {
        // do nothing
    }

    @Override
    public void setReciever(SensorListener listener) {
        eventGenerator.setListener(listener);
    }

    
}
