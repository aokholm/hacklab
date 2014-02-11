package com.vaavud.sensor.test.internal;

import com.vaavud.sensor.Sensor;
import com.vaavud.sensor.Sensor.Type;
import com.vaavud.sensor.SensorEvent3D;
import com.vaavud.sensor.SensorListener;

public class EventGenerator {
    
    private Sensor sensor;
    private SensorListener listener;
    private double Amp;
    private double Freq;
    private double SF;
    private float endTime;
    
    public EventGenerator() {
        Amp = 10d;
        Freq = 30d;
        SF = 100d;
        endTime = 30f;
        sensor = new Sensor(Type.MAGNETIC_FIELD, "TestGenerator");
    }
    
    
    public void start() {
        
        long endTimeUs = (long) (endTime * 1_000_000);
        long timeUs;
        int rateUs = (int) (1/SF * 1_000_000);
        
        for (timeUs = 0; timeUs < endTimeUs; timeUs+=rateUs) {
            newEvent(timeUs);
        }
    }
    
    
    public void newEvent(long timeUS) {
        
        double x = Amp * Math.sin(Math.PI * 2 * timeUS / 1_000_000 * Freq);
        double y = x;
        double z = x;
        listener.newEvent(new SensorEvent3D(sensor, timeUS, new double[]{x,y,z}));
    }
    
    public void setListener(SensorListener listener) {
        this.listener = listener;
    }
}
