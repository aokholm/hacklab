package com.vaavud.sensor.test;

import com.vaavud.sensor.BaseSensor;
import com.vaavud.sensor.SensorListener;
import com.vaavud.sensor.test.internal.EventGenerator;
import com.vaavud.sensor.test.internal.EventGeneratorFreq;
import com.vaavud.sensor.test.internal.EventGeneratorPlain;

public class TestSensor extends BaseSensor{
    
    private EventGenerator eventGenerator;
    
    public TestSensor(TestSensorConfig testSensorConfig) {
        TestType testType = testSensorConfig.getTestType();
        
        switch (testType) {
        case PLAIN:
            eventGenerator = new EventGeneratorPlain(testSensorConfig);
            break;
        case FREQ:    
            eventGenerator = new EventGeneratorFreq(testSensorConfig);
            break;
        default:
            eventGenerator = new EventGeneratorPlain(testSensorConfig);
            break;
        }
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
