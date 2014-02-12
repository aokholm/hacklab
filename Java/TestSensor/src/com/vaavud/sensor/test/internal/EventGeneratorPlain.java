package com.vaavud.sensor.test.internal;

import com.vaavud.sensor.test.TestSensorConfig;

public class EventGeneratorPlain extends EventGenerator {

    private double endTime = 10f;
    
    public EventGeneratorPlain(TestSensorConfig testSensorConfig) {
        super(testSensorConfig);
    }
    
    @Override
    public void start() {
        long endTimeUs = (long) (endTime * 1_000_000);
        int rateUs = (int) (1/sf * 1_000_000);
        double angle;
        
        
        for (long timeUs = 0; timeUs < endTimeUs; timeUs+=rateUs) {
            angle = Math.PI * 2 * timeUs / 1_000_000 * freq;
            
            newEvent(timeUs, angle);
        }
    }
}
